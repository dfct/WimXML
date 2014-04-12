wimxml
======

Display and Replace the XML image information in WIM files.

Output from /?:

```
WimXML - Display and Replace the XML image information in WIM files.

Version: 0.0.1

   /wimfile <file> [/showxml | /savexml <file> | /replacexml <file>]


   /wimfile <file>           Path to the WIM file
      /showxml               Show the XML image info
      /savexml <file>        Save the XML image info to a file
      /replacexml <file>     Replace stored XML image info with file contents
      /usecreateasmodified   Replace the stored Modified times with Create times


Examples:
      wimxml.exe /wimfile mywim.wim /showxml
      wimxml.exe /wimfile "C:\myFolder\install.wim" /savexml C:\myXML.xml
```

	  
Example embedded WIM XML info, this from x86 PE 5:
```
<WIM>
   <TOTALBYTES>139846944</TOTALBYTES>
   <IMAGE INDEX="1">
      <DIRCOUNT>2703</DIRCOUNT>
      <FILECOUNT>12369</FILECOUNT>
      <TOTALBYTES>862190505</TOTALBYTES>
      <HARDLINKBYTES>324280176</HARDLINKBYTES>
      <CREATIONTIME>
         <HIGHPART>0x01CE9F04</HIGHPART>
         <LOWPART>0x5F9E1B18</LOWPART>
      </CREATIONTIME>
      <LASTMODIFICATIONTIME>
         <HIGHPART>0x01CE9F04</HIGHPART>
         <LOWPART>0x607BDB5B</LOWPART>
      </LASTMODIFICATIONTIME>
      <WINDOWS>
         <ARCH>0</ARCH>
         <PRODUCTNAME>Microsoft® Windows® Operating System</PRODUCTNAME>
         <EDITIONID>WindowsPE</EDITIONID>
         <INSTALLATIONTYPE>WindowsPE</INSTALLATIONTYPE>
         <PRODUCTTYPE>WinNT</PRODUCTTYPE>
         <PRODUCTSUITE />
         <LANGUAGES>
            <LANGUAGE>en-US</LANGUAGE>
            <DEFAULT>en-US</DEFAULT>
         </LANGUAGES>
         <VERSION>
            <MAJOR>6</MAJOR>
            <MINOR>3</MINOR>
            <BUILD>9600</BUILD>
            <SPBUILD>16384</SPBUILD>
            <SPLEVEL>0</SPLEVEL>
         </VERSION>
         <SYSTEMROOT>WINDOWS</SYSTEMROOT>
      </WINDOWS>
      <NAME>Microsoft Windows PE (x86)</NAME>
      <DESCRIPTION>Microsoft Windows PE (x86)</DESCRIPTION>
   </IMAGE>
</WIM>
```

The binaries were compiled with Visual Studio 2013, so you'll need the [Visual C++ 2013 redistributables](http://www.microsoft.com/en-us/download/details.aspx?id=40784) installed to run them as is. Of course, you can also compile source.cpp with the compiler of your choice as well.

I wrote this as Windows Deployment Services shows users the Date Modified from the WIM in Windows Setup. I would much rather show the create date, but couldn't find a way to change that. So I built this to swap them and ended up generalizing it a bit to show, save, and replace the entire XML. You can read a little bit more on this at https://blog.internals.io.
