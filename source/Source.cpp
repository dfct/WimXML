//WimXML - Display and Replace the XML image information in WIM files.

#include <Windows.h>						
#include "wimgapi.h"									//WIM* APIs
#include "pugixml.hpp"									//XML APIs
#include "APIHook.h"									//Used to hook a call to GetSystemTimeAsFileTime
#include <iostream>										//Output
#include <io.h>											//This and fcntl.h are used for _setmode call to allow outputting unicode to stdout
#include <fcntl.h>										

#if _WIN64 
#pragma comment(lib, "x64_wimgapi.lib")						//x64 WIM* API library from MS
#else
#pragma comment(lib, "x86_wimgapi.lib")						//x86 WIM* API library from MS
#endif


using namespace pugi;									//xml* APIs
using std::wcout;										//Console output
using std::endl;										

wchar_t version[] = { L"0.0.1" };						//Version, printed in help output
DWORD modifiedHigh = 0;									//modifiedHigh and Low informs the myGetSystemTimeAsFileTime API hook of the time to return for the XML
DWORD modifiedLow = 0;									//They are globals as the parameters of the function need to be identical to what it hooks

void showhelp();										//Print help text to stdout
void showxml(wchar_t wimFile[]);						//Print embedded WIM XML to stdout
void savexml(wchar_t wimFile[], wchar_t saveFile[]);	//Save the embedded WIM XML to a file
void replacexml(wchar_t wimFile[], wchar_t xmlFile[]);	//Replace the embedded WIM XML with the contents of a file
void replaceModifiedwithCreate(wchar_t wimFile[]);		//Replace the embedded WIM XML's modified times with the create times
void WINAPI myGetSystemTimeAsFileTime(LPFILETIME pTime);//Function to replace kernel32's GetSystemTimeAsFileTime


//Check command line parameters and jump out to the appropriate function (or error)
int wmain(int argc, wchar_t * argv[])
{

	//Set stdout translation to unicode text. This allows us to output unicode characters
	_setmode(_fileno(stdout), _O_U16TEXT);
	wcout << endl;


	//The C++ standard requires that if there are any parameters, the first parameter will be
	//the name used to invoke the program. So argc needs to be at least two or we'll show help
	if (argc < 2)
	{
		showhelp();
		return 0;
	}

	//Wcscmp returns 0 for exact string matches so we can test for /?. 
	if (wcscmp(L"/?", argv[1]) == 0)
	{
		showhelp();
		return 0;
	}

	//We need at least 4 parameters for a valid command line other than /? and the 
	//second one must be /wimfile. Example: <program> /wimfile <filepath> <switch>
	if (argc < 4 || wcscmp(L"/wimfile", argv[1]) != 0)
	{
		wcout << "Unrecognized command line. Run /? for help." << endl;
		return 0;
	}
	

	//So far so good, let's jump out as appropriate
	if (wcscmp(L"/showxml", argv[3]) == 0)
	{
		//Call showxml with the WIM file path
		showxml(argv[2]); 
	}
	else if (wcscmp(L"/usecreateasmodified", argv[3]) == 0)
	{
		//Call replaceModifiedwithCreate with the WIM file path
		replaceModifiedwithCreate(argv[2]);
	}
	else if (wcscmp(L"/savexml", argv[3]) == 0 && argc == 5)
	{
		//Call savexml with the WIM file path and the XML file path
		savexml(argv[2], argv[4]); 
	}
	else if (wcscmp(L"/replacexml", argv[3]) == 0 && argc == 5)
	{
		//Call replacexml with the WIM file path and the XML file path
		replacexml(argv[2], argv[4]);
	}
	else
	{
		//Command line parameters was passed, but I don't recognize it
		wcout << "Unrecognized command line. Run /? for help." << endl;
		return 0;
	}
	

	return 0;
}

void showhelp()
{
	wcout << "WimXML - Display and Replace the XML image information in WIM files.\n"
		<< "\nVersion: " << version << "\n"
		<< "\n"
		<< "   /wimfile <file> [/showxml | /savexml <file> | /replacexml <file>]\n"
		<< "\n"
		<< "\n"
		<< "   /wimfile <file>           Path to the WIM file\n"
		<< "      /showxml               Show the XML image info\n"
		<< "      /savexml <file>        Save the XML image info to a file\n"
		<< "      /replacexml <file>     Replace stored XML image info with file contents\n"
		<< "      /usecreateasmodified   Replace the stored Modified times with Create times\n"
		<< "\n"
		<< "\n"
		<< "Examples: \n"
		<< "      wimxml.exe /wimfile mywim.wim /showxml\n"
		<< "      wimxml.exe /wimfile \"C:\\myFolder\\install.wim\" /savexml C:\\myXML.xml\n"
		<< "\n";

	return;
}

void showxml(wchar_t wimFile[])
{
	HANDLE hWIMFile = NULL;				//Handle for working with a WIM file 
	PVOID imageInfo = nullptr;			//Pointer to image info data
	DWORD pcbImageInfo = 0;				//The size of image info data
	xml_document xmlImageInfo;			//Pugi XML document to load the image info into


	wcout << "Opening WIM file: " << wimFile << endl;

	hWIMFile = WIMCreateFile(			//Open handle to the WIM
		wimFile,						//WIM File path
		WIM_GENERIC_WRITE,				//Read/write permission
		WIM_OPEN_EXISTING,				//Only open existing WIM, do not create one
		NULL,							//No flags
		NULL,							//No compression info 
		NULL							//No creation result info
		);

	//If hWIMFile is still NULL, the open call failed.
	if (!hWIMFile)
	{
		wcout << "Error opening the WIM file. Error code: " << GetLastError() << endl;
		ExitProcess(GetLastError());
	}

	wcout << "Printing WIM XML image info:" << endl << endl;

	WIMGetImageInformation(				//Get image info
		hWIMFile,						//Handle to the WIM
		&imageInfo,						//Address to store the pointer
		&pcbImageInfo					//Address to store the size of the image info data
		);

	xmlImageInfo.load_buffer(			//Load the XML info into Pugi document
		imageInfo,						//Pointer to image info data
		pcbImageInfo					//The size of image info data
		);


	
	xmlImageInfo.save(					//And write out the contents 
		wcout,							//To wcout
		L"   ",							//Using three spaces as indentation, mirroring how the XML is stored 
		format_no_declaration |			//Tell pugi not to add its own <?xml definition
		format_indent					//And to use my indentation
		);			


	wcout << endl << "Done." << endl;	//Print done
	WIMCloseHandle(hWIMFile);			//Close the handle to the WIM file
	ExitProcess(0);						//And exit
}

void savexml(wchar_t wimFile[], wchar_t saveFile[])
{
	HANDLE hWIMFile = NULL;				//Handle for working with a WIM file 
	PVOID imageInfo = nullptr;			//Pointer to image info data
	DWORD pcbImageInfo = 0;				//The size of image info data
	xml_document xmlImageInfo;			//Pugi XML document to load the image info into


	wcout << "Opening WIM file: " << wimFile << endl;

	hWIMFile = WIMCreateFile(			//Open handle to the WIM
		wimFile,						//WIM File path
		WIM_GENERIC_WRITE,				//Read/write permission
		WIM_OPEN_EXISTING,				//Only open existing WIM, do not create one
		NULL,							//No flags
		NULL,							//No compression info 
		NULL							//No creation result info
		);

	//If hWIMFile is still NULL, the open call failed.
	if (!hWIMFile)
	{
		wcout << "Error opening the WIM file. Error code: " << GetLastError() << endl;
		ExitProcess(GetLastError());
	}

	wcout << "Saving WIM XML image info to file: " << saveFile << endl;

	WIMGetImageInformation(				//Get image info
		hWIMFile,						//Handle to the WIM
		&imageInfo,						//Address to store the pointer
		&pcbImageInfo					//Address to store the size of the image info data
		);

	xmlImageInfo.load_buffer(			//Load the XML info into Pugi document
		imageInfo,						//Pointer to image info data
		pcbImageInfo					//The size of image info data
		);

	bool saved = xmlImageInfo.save_file(//And write the contents to a file
		saveFile,						//To the file saveFile
		L"   ", 						//Using three spaces as indentation, mirroring how the XML is stored 
		format_no_declaration | 		//Tell pugi not to add its own <?xml definition
		format_indent | 				//And to use my indentation
		format_write_bom,				//And to write otu the bom indicating the encoding type
		encoding_utf16_le				//Which is utf16_le
		);	

	//Make sure we got back true
	if (!saved)
	{
		wcout << "Error saving XML to file: " << saveFile << endl;
		ExitProcess(1);
	}

	//Write out that we finished and the path
	wcout << "Saved XML to file: " << saveFile << endl;
	
	//Close the handle to the WIM
	WIMCloseHandle(hWIMFile);
	
	//And exit
	ExitProcess(0);

}

void replacexml(wchar_t wimFile[], wchar_t xmlFile[])
{
	wchar_t * xml = nullptr;			//Pointer to read the XML into
	DWORD fileSize = 0;					//DWORD to store the fileSize
	xml_document xmlModifiedTimes;		//Pugi xml_document to load the XML data into for parsing modified times
	HANDLE hWIMFile = NULL;				//Handle for working with a WIM file 


	wcout << "Opening XML file: " << xmlFile << endl;

	HANDLE xmlHandle = CreateFile(		//Get a handle to the xmlFile
		xmlFile,						//File path
		GENERIC_READ,					//Read access
		FILE_SHARE_READ,				//Share read access with other users
		NULL,							//Don't need to deal with specific security info
		OPEN_EXISTING,					//Only open the file if it exists, do not create a new one
		FILE_ATTRIBUTE_NORMAL,			//Looking for a file with normal attributes
		NULL
		);

	//Ensure the handle is valid
	if (xmlHandle == INVALID_HANDLE_VALUE)
	{
		wcout << "Error opening the XML file. Error code: " << GetLastError() << endl;
		ExitProcess(GetLastError());
	}

	
	fileSize = GetFileSize(				//Get the size of the file and store it in fileSize
		xmlHandle,						//File handle
		NULL							//Don't need to store the data this way
		);

	//Ensure we got back valid data
	if (fileSize == INVALID_FILE_SIZE)
	{
		wcout << "Error getting the size of the XML file. Error code: " << GetLastError() << endl;
		ExitProcess(GetLastError());
	}


	xml = new wchar_t[fileSize / 2];	//Allocate a wchar_t[] array of the size of the file (halved as wchar_t are two-bytes)
	DWORD readAmount = 0;				//And a DWORD for the ReadFile function to inform what it read

	ReadFile(							//And read the contents into the buffer
		xmlHandle,						//File handle
		(PVOID)xml,						//Pointer to the wchar_t buffer
		fileSize,						//Amount of data to read
		&readAmount,					//Pointer to the readAmount DWORD for it to store what it read
		NULL							//Not issuing overlapped IO
		);

	xml[fileSize / 2] = '\0';			//And ensure the last character is a null


	if (xml[0] != 0xFEFF)				//Test if the file is encoded as UTF-16 little endian (required by the WIM functions)
	{
		wcout << "Error reading XML file. The XML file needs to be encoded as UTF-16 Little Endian. " << endl;
		ExitProcess(1);
	}
	

	//Load the XML buffer into the pugi xml_document
	xmlModifiedTimes.load_buffer(xml, fileSize);

	//Create xml_text representations of the High and Low modified times
	xml_text mHigh = xmlModifiedTimes.child(L"WIM").child(L"IMAGE").child(L"LASTMODIFICATIONTIME").child(L"HIGHPART").text();
	xml_text mLow = xmlModifiedTimes.child(L"WIM").child(L"IMAGE").child(L"LASTMODIFICATIONTIME").child(L"LOWPART").text();

	//And set the global variables to their values
	//wcstoul converts the text to a ulong (DWORD), starting at index 2 to dodge 0x., ending at the null, and in base 16
	modifiedHigh = wcstoul(&mHigh.get()[2], L'\0', 16);
	modifiedLow = wcstoul(&mLow.get()[2], L'\0', 16);
	

	//Now, initialize the hook to GetSystemTimeAsFileTime
	CAPIHook g_GetSystemTimeAsFileTime("Kernel32.dll", "GetSystemTimeAsFileTime", (PROC)myGetSystemTimeAsFileTime);


	//And open the WIM
	wcout << "Opening WIM file: " << wimFile << endl;

	hWIMFile = WIMCreateFile(			//Open handle to the WIM
		wimFile,						//WIM File path
		WIM_GENERIC_WRITE,				//Read/write permission
		WIM_OPEN_EXISTING,				//Only open existing WIM, do not create one
		NULL,							//No flags
		NULL,							//No compression info 
		NULL							//No creation result info
		);

	//If hWIMFile is still NULL, the open call failed.
	if (!hWIMFile)
	{
		wcout << "Error opening the WIM file. Error code: " << GetLastError() << endl;
		ExitProcess(GetLastError());
	}


	//Modifying the WIM requires a working directory, so let's expand %temp% and use that
	wchar_t tempFolder[260] = {};		//wchar_t to store the path
	ExpandEnvironmentStringsW(			//Expand the environment variable
		L"%temp%",						//%temp%
		tempFolder,						//Info tempFolder
		260								//Buffer size
		);

	WIMSetTemporaryPath(				//Set the temporary path
	hWIMFile,							//For this WIM handle
	tempFolder							//To this directory
	);


	HRESULT hr = WIMSetImageInformation(//Now save XML image info into the WIM
		hWIMFile,						//Handle to the WIM
		xml,							//Address of the XML data
		fileSize						//Size of the XML data
		);

	//And check if it worked
	if (hr == 0)
	{
		wcout << "Error saving XML file. Error: " << GetLastError() << endl;
		ExitProcess(GetLastError());
	}
	
	//Close the handle
	hr = WIMCloseHandle(hWIMFile);

	//And check if *that* worked, as that does way more work than CloseHandle implies
	if (hr == 0)
	{
		wcout << "Error saving XML file. Error: " << GetLastError() << endl;
		ExitProcess(GetLastError());
	}

	//Success! Wrap up
	wcout << endl << "Success." << endl;

	//And exit process
	ExitProcess(0);
}

void replaceModifiedwithCreate(wchar_t wimFile[])
{
	HANDLE hWIMFile = NULL;				//Handle for working with a WIM file 
	PVOID imageInfo = nullptr;			//Pointer to image info data
	DWORD pcbImageInfo = 0;				//The size of image info data
	xml_document xmlImageInfo;			//Pugi XML document to load the image info into

	//Initialize the hook before all WIM calls
	//This will cause some times to be 0x000000 before I fill the global variables. They will be overwritten again later, so this will be ok
	CAPIHook g_GetSystemTimeAsFileTime("Kernel32.dll", "GetSystemTimeAsFileTime", (PROC)myGetSystemTimeAsFileTime);

	wcout << "Opening WIM file: " << wimFile << endl;

	hWIMFile = WIMCreateFile(			//Open handle to the WIM
		wimFile,						//WIM File path
		WIM_GENERIC_WRITE,				//Read/write permission
		WIM_OPEN_EXISTING,				//Only open existing WIM, do not create one
		NULL,							//No flags
		NULL,							//No compression info 
		NULL							//No creation result info
		);

	//If hWIMFile is still NULL, the open call failed.
	if (!hWIMFile)
	{
		wcout << "Error opening the WIM file. Error code: " << GetLastError() << endl;
		ExitProcess(GetLastError());
	}

	WIMGetImageInformation(				//Get image info
		hWIMFile,						//Handle to the WIM
		&imageInfo,						//Address to store the pointer
		&pcbImageInfo					//Address to store the size of the image info data
		);

	xmlImageInfo.load_buffer(			//Load the XML info into Pugi document
		imageInfo,						//Pointer to image info data
		pcbImageInfo					//The size of image info data
		);


	//Create xml_text representations of the High and Low creation times
	xml_text mHigh = xmlImageInfo.child(L"WIM").child(L"IMAGE").child(L"CREATIONTIME").child(L"HIGHPART").text();
	xml_text mLow = xmlImageInfo.child(L"WIM").child(L"IMAGE").child(L"CREATIONTIME").child(L"LOWPART").text();

	//And set the global variables to their values
	//wcstoul converts the text to a ulong (DWORD), starting at index 2 to dodge 0x., ending at the null, and in base 16
	modifiedHigh = wcstoul(&mHigh.get()[2], L'\0', 16);
	modifiedLow = wcstoul(&mLow.get()[2], L'\0', 16);



	//Modifying the WIM requires a working directory, so let's expand %temp% and use that
	wchar_t tempFolder[260] = {};		//wchar_t to store the path
	ExpandEnvironmentStringsW(			//Expand the environment variable
		L"%temp%",						//%temp%
		tempFolder,						//Info tempFolder
		260								//Buffer size
		);

	WIMSetTemporaryPath(				//Set the temporary path
		hWIMFile,						//For this WIM handle
		tempFolder						//To this directory
		);


	//Now save XML image info into the WIM
	HRESULT hr = WIMSetImageInformation(
		hWIMFile,						//Handle to the WIM
		imageInfo,						//Address of the XML data
		pcbImageInfo					//Size of the XML data
		);

	//And check if it worked
	if (hr == 0)
	{
		wcout << "Error saving XML file. Error: " << GetLastError() << endl;
		ExitProcess(GetLastError());
	}

	//Close the handle
	hr = WIMCloseHandle(hWIMFile);

	//And check if *that* worked, as that does way more work than CloseHandle implies
	if (hr == 0)
	{
		wcout << "Error saving XML file. Error: " << GetLastError() << endl;
		ExitProcess(GetLastError());
	}

	//Success! Wrap up
	wcout << endl << "Success." << endl;

	//And exit process
	ExitProcess(0);

}

void WINAPI myGetSystemTimeAsFileTime(_Out_ LPFILETIME lpSystemTimeAsFileTime)
{
	lpSystemTimeAsFileTime->dwHighDateTime = modifiedHigh;
	lpSystemTimeAsFileTime->dwLowDateTime = modifiedLow;
	return;
}

