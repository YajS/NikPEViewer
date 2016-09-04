#pragma once

TCHAR DOS_HEADER_E_LFANEW[]             = _T("File address of new exe header");
TCHAR DOS_HEADER_E_OEMINFO[]            = _T("OEM information (e_oemid specific)");
TCHAR DOS_HEADER_E_OEMID[]              = _T("OEM identifier (for e_oemid)");
TCHAR DOS_HEADER_E_OVNO[]               = _T("Overlay number");
TCHAR DOS_HEADER_E_LFARLC[]             = _T("File address of relocation table");
TCHAR DOS_HEADER_E_CS[]                 = _T("Initial (relative) CS value");
TCHAR DOS_HEADER_E_IP[]                 = _T("Initial IP value");
TCHAR DOS_HEADER_E_CSUM[]               = _T("Checksum");
TCHAR DOS_HEADER_E_SP[]                 = _T("Initial SP value");
TCHAR DOS_HEADER_E_SS[]                 = _T("Initial (relative) SS value");
TCHAR DOS_HEADER_E_MAXALLOC[]           = _T("Maximum extra paragraphs needed");
TCHAR DOS_HEADER_E_MINALLOC[]           = _T("Minimum extra paragraphs needed");
TCHAR DOS_HEADER_E_CPARHDR[]            = _T("Size of header in paragraphs");
TCHAR DOS_HEADER_E_CRLC[]               = _T("Relocations");
TCHAR DOS_HEADER_E_CP[]                 = _T("Pages in file");
TCHAR DOS_HEADER_E_CBLP[]               =  _T("Bytes on last page of file");
TCHAR DOS_HEADER_E_MAGIC[]              = _T("Magic number");
TCHAR DOS_HEADER_1[]                    = _T("Dos header");
TCHAR STR_PRINTF_X[]                    = _T("0x%X");
TCHAR STR_PRINTF_s[]                    = _T("%s");
TCHAR STR_HEX_VALUES[]                  = _T("Hex values");
TCHAR STR_DESCRIPTION[]                 = _T("Description");

TCHAR NT_HEADERS_1[]                    = _T("NT headers");
TCHAR NT_HEADERS_SIGNATURE[]            = _T("Signature");

TCHAR NT_HEADERS_FH_1[]                 = _T("File headers");
TCHAR NT_HEADERS_FH_2[]                 = _T("NT File headers");
TCHAR NT_HEADERS_FH_MACHINE[]           = _T("Machine");
TCHAR NT_HEADERS_FH_NUOFSECTIONS[]      = _T("NumberOfSections");
TCHAR NT_HEADERS_FH_TIMEDATESTAMP[]     = _T("TimeDateStamp");
TCHAR NT_HEADERS_FH_PTSYMBOLTABLE[]     = _T("PointerToSymbolTable");
TCHAR NT_HEADERS_FH_NUMBEROFSYMBOL[]    = _T("NumberOfSymbols");
TCHAR NT_HEADERS_FH_SIZEOFOPHEADER[]    = _T("SizeOfOptionalHeader");
TCHAR NT_HEADERS_FH_CHARACTERISTIC[]    = _T("Characteristics");

TCHAR NT_HEADERS_OP_NUOFRVAANDSIZES[]   = _T("NumberOfRvaAndSizes");
TCHAR NT_HEADERS_OP_LOADERFLAGS[]       = _T("LoaderFlags");
TCHAR NT_HEADERS_OP_SIZEOFHEAPCOMMIT[]  = _T("SizeOfHeapCommit");
TCHAR NT_HEADERS_OP_SIZEOFHEAPRESERVE[] = _T("SizeOfHeapReserve");
TCHAR NT_HEADERS_OP_SIZEOFSTACKCOMMIT[] = _T("SizeOfStackCommit");
TCHAR NT_HEADERS_OP_SIZEOFSTACKRESERVE[]= _T("SizeOfStackReserve");
TCHAR NT_HEADERS_OP_SUBSYSTEM[]         = _T("Subsystem");
TCHAR NT_HEADERS_OP_CHECKSUM[]          = _T("CheckSum");
TCHAR NT_HEADERS_OP_DLLCHARACTERISTIC[] = _T("DllCharacteristics");
TCHAR NT_HEADERS_OP_WDMDRIVER[]         = _T("WDM driver");
TCHAR NT_HEADERS_OP_SIZEOFHEADERS[]     = _T("SizeOfHeaders");
TCHAR NT_HEADERS_OP_SIZEOFIMAGE[]       = _T("SizeOfImage");
TCHAR NT_HEADERS_OP_WIN32VERSIONVALUE[] = _T("Win32VersionValue");
TCHAR NT_HEADERS_OP_MINORSUBSYSVERSION[]= _T("MinorSubsystemVersion");
TCHAR NT_HEADERS_OP_MAJORSUBSYSVERSION[]= _T("MajorSubsystemVersion");
TCHAR NT_HEADERS_OP_MINORIMAGEVERSION[] = _T("MinorImageVersion");
TCHAR NT_HEADERS_OP_MAJORLINKERVERSION[]= _T("MajorLinkerVersion");
TCHAR NT_HEADERS_OP_MINOROSVERSION[]    = _T("MinorOperatingSystemVersion");
TCHAR NT_HEADERS_OP_MAJOROSVERSION[]    = _T("MajorOperatingSystemVersion");
TCHAR NT_HEADERS_OP_FILEALIGNMENT[]     = _T("FileAlignment");
TCHAR NT_HEADERS_OP_SECTIONALIGNMENT[]  = _T("SectionAlignment");
TCHAR NT_HEADERS_OP_IMAGEBASE[]         = _T("ImageBase");
TCHAR NT_HEADERS_OP_BASEOFDATA[]        = _T("BaseOfData");
TCHAR NT_HEADERS_OP_BASEOFCODE[]        = _T("BaseOfCode");
TCHAR NT_HEADERS_OP_ADDOFENTRYPOINT[]   = _T("AddressOfEntryPoint");
TCHAR NT_HEADERS_OP_SIZEOFUNINITDATA[]  = _T("SizeOfUninitializedData");
TCHAR NT_HEADERS_OP_SIZEOFINITDATA[]    = _T("SizeOfInitializedData");
TCHAR NT_HEADERS_OP_SIZEOFCODE[]        = _T("SizeOfCode");
TCHAR NT_HEADERS_OP_MINORLINKERVERSION[]= _T("MinorLinkerVersion");
TCHAR NT_HEADERS_OP_MAGIC[]             = _T("Magic");
TCHAR NT_HEADERS_OP_1[]                 = _T("Optional headers");
TCHAR NT_HEADERS_OP_2[]                 = _T("NT Optional headers");

TCHAR STR_ORDIANL_NO[]                  = _T("Ordianl No");
TCHAR STR_ENTRY_POINT[]                 = _T("Entry Point");
TCHAR STR_FUNCTION_NAME[]               = _T("Function name");
TCHAR STR_EXPORT_FUNCTIONS[]            =_T("Export functions");

TCHAR STR_VIRTUAL_ADDRESS[]             = _T("Virtual Address");
TCHAR STR_DATADIRECTORY_SIZE[]          = _T("DataDirectory[%d].Size");
TCHAR STR_DATA_DIRECTORY[]              = _T("Data Directory");

TCHAR SH_NUMBEROFLINENUMBERS[]          = _T("NumberOfLinenumbers");
TCHAR SH_NUMBEROFRELOCATIONS[]          = _T("NumberOfRelocations");
TCHAR SH_POINTERTOLINENUMBERS[]         = _T("PointerToLinenumbers");
TCHAR SH_POINTERTORELOCATIONS[]         = _T("PointerToRelocations");
TCHAR SH_POINTERTORAWDATA[]             = _T("PointerToRawData");
TCHAR SH_SIZEOFRAWDATA[]                = _T("SizeOfRawData");
TCHAR SH_VIRTUAL_ADDRESS[]              = _T("Virtual Address");
TCHAR SH_PHYSICAL_ADDRESS[]             = _T("Physical Address");
TCHAR SH_SECTIONS_NAME[]                = _T("Sections name");
TCHAR SH_SECTIONS_LIST[]                = _T("Sections list");

TCHAR STR_RECENT_FILES[]                = _T("Recent Files");

TCHAR STR_HTML_START[]                  = _T("<html>");
TCHAR STR_HTML_END[]                    = _T("</html>");

TCHAR STR_HTML_HEAD[]                   = _T("<head><title>PORTABLE EXECUTABLE FORMAT</title><style type=\"text/css\"> \r\n\
<!-- A:HOVER {font-weight:bold;color:#3399FF} --></style></head>");

//TCHAR STR_HTML_BODY_S[]                  = _T("<body leftmargin=\"50\" bgcolor=\"#000000\" text=\"#e0e0e0\" link=\"#d0d0ff\" vlink=\"#ffd0d0\"> \r\n<br><center><br><h3>THE<BR>PORTABLE EXECUTABLE FORMAT</h3><br><hr><br></center><br>");

TCHAR STR_HTML_BODY_S[]                  = _T("<body leftmargin=\"50\" bgcolor=\"#000000\" text=\"#e0e0e0\" link=\"#d0d0ff\" vlink=\"#ffd0d0\"> \r\n<br>");

TCHAR STR_HTML_BODY_S_1[]                  = _T("<body leftmargin=\"50\" bgcolor=\"#000000\" text=\"#e0e0e0\" link=\"#d0d0ff\" vlink=\"#ffd0d0\"> \r\n<br>");

TCHAR STR_HTML_BODY_E[]                  = _T("</body>");

TCHAR STR_HTML_TABLE_S[]                 = _T("<table border=\"0\" bordercolor=\"#AAAAAA\" cellpadding=\"2\" cellspacing=\"0\" bgcolor=\"#102030\" align=\"center\">");
TCHAR STR_HTML_TABLE_E[]                 = _T("</table>");

TCHAR STR_HTML_TB_TR_S[]                 = _T("<tr>");
TCHAR STR_HTML_TB_TR_E[]                 = _T("</tr>");

TCHAR STR_HTML_TB_TD_CENTER[]            = _T("<td width=\"400\" ColSpan=\"2\" style=\"text-align: center\">");
TCHAR STR_HTML_TB_TD_S[]                 = _T("<td>");
TCHAR STR_HTML_TB_TD_E[]                 = _T("</td>");

TCHAR STR_LICENSE[]                     = _T("Protable Executable (PE) 32-bits file Viewer, Bita Version 0.0.0.1\r\n\r\n\
CodeDebug grants you a personal right to install and execute the Product \
on a Host Computer for  Personal Use or Educational Use or for Evaluation.\r\n\r\n\
Since you have not paid for the use of the Product, there is no warranty for it, to \
the extent permitted by applicable law. \
This software is provided \"as is\" and use of \
the software is at your own risk. \'CodeDebug\' disclaims \
any and all warranties, whether express, implied or statutory \
, including, without limitation, any implied warranties of \
merchantability, fitness for a particular purpose or non-infringement \
of third-party rights. \'CodeDebug\' does not warrant that the software is free of Defects.\r\n\r\n\
You are not allowed to modify, decompile, disassemble or reverse engineer \
the Software except and only to the extent that such activity is expressly \
permitted by applicable law. You are not allowed to distribute or use any \
parts of the Software separately.\r\n\r\nYou may not use the Software or any Content \
for any commercial purpose.");

/*TCHAR STR_HTML_TEXT[]                     = _T("<html>  \r\n\
<head>                                                  \r\n\
    <title>PE NikViewer v1.0</title>                    \r\n\
</head>                                                 \r\n\
<frameset rows=\"10%,90%\">                             \r\n\
<frame noresize=\"noresize\" src=\"about:blank\" />     \r\n\
<frameset cols=\"20%,80%\">                             \r\n\
<frame src=\"about:blank\" />                           \r\n\
<frame src=\"petest.html\" />                           \r\n\
</frameset>                                             \r\n\
</frameset>                                             \r\n\*/
