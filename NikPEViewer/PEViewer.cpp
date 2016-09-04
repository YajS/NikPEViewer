
#include "Stdafx.h"
#include "Resource.h"
#include "PEViewer.h"

CNikPEViewer * g_pNikPEViewer;

extern HWND g_hwndStatus;
extern TCHAR STR_HTML_TEXT[];
extern TCHAR STR_PRINTF_X[];
extern TCHAR STR_PRINTF_s[];
extern CNikDisassembly g_cNikDisassembly;

const TCHAR  g_szNikPEViewerClass[]  = _T("NIK_PEVIEWER");
// The predefined resource types
const TCHAR *g_chResourceTypes[] = {_T("???_0")
							        , _T("CURSOR")
							        , _T("BITMAP")
							        , _T("ICON")
							        , _T("MENU")
							        , _T("DIALOG")
							        , _T("STRING")
							        , _T("FONTDIR")
							        , _T("FONT")
							        , _T("ACCELERATORS")
							        , _T("RCDATA")
							        , _T("MESSAGETABLE")
							        , _T("GROUP_CURSOR")
							        , _T("???_13")
							        , _T("GROUP_ICON")
							        , _T("???_15")
							        , _T("VERSION")};

CNikPEViewer::CNikPEViewer():
      m_hWnd(NULL)
      , m_nChildCount(0)
      , m_nDiffOffset(0)
      , m_hInstance(NULL)
      , m_hTreeCtrl(NULL)
      , m_pPEBuffer(NULL)
      , m_dwPEFileSize(0)
      , m_dwDiffAddress(0)
      , m_pstResourceList(NULL)
      , m_dwResourceBaseAdd(0)
{
}

CNikPEViewer::~CNikPEViewer()
{
    m_vImportDllList.clear();
    m_vExportFunctionList.clear();

    if(m_hWnd)
       DestroyWindow( m_hWnd );

    if(m_pPEBuffer)
        delete [] m_pPEBuffer;

    if(m_pstResourceList)
		delete m_pstResourceList;
}

int CNikPEViewer::RegisterNikWindows( HINSTANCE hInstance )
{
    m_hInstance = hInstance;

    BOOL        bResult;
    WNDCLASS    wc;

    memset(&wc, 0, sizeof(WNDCLASS));
    wc.lpfnWndProc   = (WNDPROC)PEViewerWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 8;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon( hInstance, MAKEINTRESOURCE(IDI_NIKPEVIEWER) );
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szNikPEViewerClass;

	bResult = RegisterClass( &wc );
	//if(!bResult) return -1;

    CNikDialog::RegisterDialogclass( hInstance );
    CNikResource::RegisterResourceclass( hInstance );
    CNikBitmapEditor::RegisterEditorclass( hInstance );
    CNikDisassemblyWin::NikDissembeRegisterClass( hInstance );

    return 0;
}

int CNikPEViewer::GetMachineName(int nCode, int nStrSize, TCHAR *chMachine)
{
    if (0x14d == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Intel i860"));
    else if (0x14c == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Intel i386"), 1, 1);
    else if (0x162 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("MIPS R3000"), 1, 1);
    else if (0x166 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("MIPS R4000"), 1, 1);
    else if (0x183 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("DEC Alpha AXP"), 1, 1);
    else if (0x0200 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Intel IPF x64"), 1, 1);
    else if (0x8664 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("x64"), 1, 1);
	else if (0x1d3 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Matsushita AM33"), 1, 1);
	else if (0x1c0 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("ARM little endian"), 1, 1);
	else if (0x1c4 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("ARMv7 (or higher) Thumb mode only"), 1, 1);
	else if (0xebc == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("EFI byte code"), 1, 1);
	else if (0x9041 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Mitsubishi M32R little endian"), 1, 1);
	else if (0x266 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("MIPS16"), 1, 1);
	else if (0x366 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("MIPS with FPU"), 1, 1);
	else if (0x466 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("MIPS16 with FPU"), 1, 1);
	else if (0x1f0 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Power PC little endian"), 1, 1);
	else if (0x1f1 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Power PC with floating point support"), 1, 1);
	else if (0x1a2 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Hitachi SH3"), 1, 1);
	else if (0x1a3 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Hitachi SH3 DSP"), 1, 1);
	else if (0x1a6 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Hitachi SH4"), 1, 1);
	else if (0x1a8 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Hitachi SH5"), 1, 1);
	else if (0x1c2 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("ARM or Thumb (“interworking”)"), 1, 1);
	else if (0x169 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("MIPS little-endian WCE v2"), 1, 1);
    else
        _stprintf_s(chMachine, 1023, _T("Machine code: 0x%X"), nCode);

    return TRUE;
}

int CNikPEViewer::GetStrDateTimeStamp(DWORD dwDateTime, int nStrSize, TCHAR *chBuf)
{
    DWORD dwDateTime1[2];
    dwDateTime1[0] = dwDateTime;
    dwDateTime1[1] = 0;
    int err = _wctime_s(chBuf, nStrSize, (time_t *)&dwDateTime1[0]);
	if(err || (!dwDateTime))
		_stprintf_s(chBuf, nStrSize, STR_PRINTF_X, dwDateTime);
	else
	{
		err = _tcslen(chBuf);
		if(err > 1)
			chBuf[err-1] = 0;
	}

    return TRUE;
}

int CNikPEViewer::GetNTCharacteristics(WORD wCharacteristics, _tstring &strChar)
{
    if(IMAGE_FILE_RELOCS_STRIPPED & wCharacteristics)
        strChar += _T("RELOCS_STRIPPED <br/> ");
    if(IMAGE_FILE_EXECUTABLE_IMAGE & wCharacteristics)
        strChar += _T("EXECUTABLE_IMAGE <br/> ");
    if(IMAGE_FILE_LINE_NUMS_STRIPPED & wCharacteristics)
        strChar += _T("LINE_NUMS_STRIPPED <br/> ");
    if(IMAGE_FILE_LOCAL_SYMS_STRIPPED & wCharacteristics)
        strChar += _T("LOCAL_SYMS_STRIPPED <br/> ");
    if(IMAGE_FILE_AGGRESIVE_WS_TRIM & wCharacteristics)
        strChar += _T("AGGRESIVE_WS_TRIM <br/> ");
    if(IMAGE_FILE_LARGE_ADDRESS_AWARE & wCharacteristics)
        strChar += _T("LARGE_ADDRESS_AWARE <br/> ");
    if(IMAGE_FILE_BYTES_REVERSED_LO & wCharacteristics)
        strChar += _T("BYTES_REVERSED_LO <br/> ");
    if(IMAGE_FILE_32BIT_MACHINE & wCharacteristics)
        strChar += _T("32BIT_MACHINE <br/> ");
    if(IMAGE_FILE_DEBUG_STRIPPED & wCharacteristics)
        strChar += _T("DEBUG_STRIPPED <br/> ");
    if(IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP & wCharacteristics)
        strChar += _T("REMOVABLE_RUN_FROM_SWAP <br/> ");
    if(IMAGE_FILE_NET_RUN_FROM_SWAP & wCharacteristics)
        strChar += _T("NET_RUN_FROM_SWAP <br/> ");
    if(IMAGE_FILE_SYSTEM & wCharacteristics)
        strChar += _T("SYSTEM <br/> ");
    if(IMAGE_FILE_DLL & wCharacteristics)
        strChar += _T("DLL <br/> ");
    if(IMAGE_FILE_UP_SYSTEM_ONLY & wCharacteristics)
        strChar += _T("UP_SYSTEM_ONLY <br/> ");
    if(IMAGE_FILE_BYTES_REVERSED_HI & wCharacteristics)
        strChar += _T("BYTES_REVERSED_HI <br/> ");

    return TRUE;
}

int CNikPEViewer::GetNTOptionalSubsystem(int nCode, int nStrSize, TCHAR *chMachine)
{
    if(1 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("NATIVE"), 1, 1);
    else if(2 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("WINDOWS GUI"), 1, 1);
    else if(3 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("WINDOWS CONSOLE"), 1, 1);
    else if(5 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("OS2 CONSOLE"), 1, 1);
    else if(7 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("POSIX CONSOLE"), 1, 1);
    else if(9 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Windows CE"), 1, 1);
	else if(10 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("EFI application"), 1, 1);
	else if(11 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("EFI driver with boot services"), 1, 1);
	else if(10 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("EFI driver with run-time services"), 1, 1);
	else if(13 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("EFI ROM image"), 1, 1);
    else if(14 == nCode)
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("Xbox system"), 1, 1);
    else
        _stprintf_s(chMachine, nStrSize, STR_PRINTF_X, nCode);

    return TRUE;
}

int CNikPEViewer::GetDirectoryEnteries(int i, int nStrSize, TCHAR *chMachine)
{
    if(0 == i)
		_stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("EXPORT TABLE"));
	else if(1 == i)
		_stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("IMPORT TABLE"));
	else if(2 == i)
		_stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("RESOURCE TABLE"));
	else if(3 == i)
		_stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("EXCEPTION TABLE"));
	else if(4 == i)
		_stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("SECURITY TABLE"));
	else if(5 == i)
		_stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("BASE-RELOC TABLE"));
	else if(6 == i)
		_stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("DEBUG TABLE"));
	else if(7 == i)
		_stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("COPYRIGHT TABLE"));
	else if(8 == i)
		_stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("GLOBALPTR TABLE"));
	else if(9 == i)
		_stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("TLS TABLE"));
	else if(10 == i)
		_stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("LOAD_CONFIG TABLE"));
	else
		_stprintf_s(chMachine, nStrSize, STR_PRINTF_s, _T("UNDEFINED TABLE"));

    return 0;
}

int CNikPEViewer::GetNTSectionCharacteristics(DWORD dwCharacteristics, _tstring &strChar)
{
    if(IMAGE_SCN_CNT_CODE & dwCharacteristics)
        strChar += _T("CODE DATA <br/> ");
    if(IMAGE_SCN_CNT_INITIALIZED_DATA & dwCharacteristics)
        strChar += _T("INITIALIZED DATA <br/> ");
    if(IMAGE_SCN_CNT_UNINITIALIZED_DATA & dwCharacteristics)
        strChar += _T("UNINITIALIZED DATA <br/> ");
    if(IMAGE_SCN_LNK_COMDAT & dwCharacteristics)
        strChar += _T("COMDAT DATA | ");
    if(IMAGE_SCN_LNK_NRELOC_OVFL & dwCharacteristics)
        strChar += _T("EXTENDED RELOCATIONS DATA <br/> ");
    if(IMAGE_SCN_MEM_DISCARDABLE & dwCharacteristics)
        strChar += _T("DISCARDABLE DATA <br/> ");
    if(IMAGE_SCN_MEM_NOT_CACHED & dwCharacteristics)
        strChar += _T("NOT CACHED DATA <br/> ");
    if(IMAGE_SCN_MEM_SHARED & dwCharacteristics)
        strChar += _T("SHARED-ABLE DATA <br/> ");
    if(IMAGE_SCN_MEM_EXECUTE & dwCharacteristics)
        strChar += _T("EXECUTE-ABLE DATA <br/> ");
    if(IMAGE_SCN_MEM_READ & dwCharacteristics)
        strChar += _T("READ-ABLE DATA <br/> ");
    if(IMAGE_SCN_MEM_WRITE & dwCharacteristics)
        strChar += _T("WRITE-ABLE DATA <br/> ");

    return TRUE;
}

_tstring CNikPEViewer::GetVerResLanguage( DWORD dwResLang )
{
    TCHAR chBuf[1024];
    chBuf[0] = 0;

    switch(dwResLang>>0x10)
	{
	case 0x0401:
		_stprintf_s(chBuf,1023,_T("Arabic"));
		break;
	case 0x0402:
		_stprintf_s(chBuf,1023,_T("Bulgarian"));
		break;
	case 0x0403:
		_stprintf_s(chBuf,1023,_T("Catalan"));
		break;
	case 0x0404:
		_stprintf_s(chBuf,1023,_T("Traditional Chinese"));
		break;
	case 0x0405:
		_stprintf_s(chBuf,1023,_T("Czech"));
		break;
	case 0x0406:
		_stprintf_s(chBuf,1023,_T("Danish"));
		break;
	case 0x0407:
		_stprintf_s(chBuf,1023,_T("German"));
		break;
	case 0x0408:
		_stprintf_s(chBuf,1023,_T("Greek"));
		break;
	case 0x0409:
		_stprintf_s(chBuf,1023,_T("U.S. English"));
		break;
	case 0x040A:
		_stprintf_s(chBuf,1023,_T("Castilian Spanish"));
		break;
	case 0x040B:
		_stprintf_s(chBuf,1023,_T("Finnish"));
		break;
	case 0x040C:
		_stprintf_s(chBuf,1023,_T("French"));
		break;
	case 0x040D:
		_stprintf_s(chBuf,1023,_T("Hebrew"));
		break;
	case 0x040E:
		_stprintf_s(chBuf,1023,_T("Hungarian"));
		break;
	case 0x040F:
		_stprintf_s(chBuf,1023,_T("Icelandic"));
		break;
	case 0x0410:
		_stprintf_s(chBuf,1023,_T("Italian"));
		break;
	case 0x0411:
		_stprintf_s(chBuf,1023,_T("Japanese"));
		break;
	case 0x0412:
		_stprintf_s(chBuf,1023,_T("Korean"));
		break;
	case 0x0413:
		_stprintf_s(chBuf,1023,_T("Dutch"));
		break;
	case 0x0414:
		_stprintf_s(chBuf,1023,_T("Norwegian – Bokmal"));
		break;
	case 0x0810:
		_stprintf_s(chBuf,1023,_T("Swiss Italian"));
		break;
	case 0x0813:
		_stprintf_s(chBuf,1023,_T("Belgian Dutch"));
		break;
	case 0x0814:
		_stprintf_s(chBuf,1023,_T("Norwegian – Nynorsk"));
		break;
	case 0x0415:
		_stprintf_s(chBuf,1023,_T("Polish"));
		break;
	case 0x0416:
		_stprintf_s(chBuf,1023,_T("Portuguese (Brazil)"));
		break;
	case 0x0417:
		_stprintf_s(chBuf,1023,_T("Rhaeto-Romanic"));
		break;
	case 0x0418:
		_stprintf_s(chBuf,1023,_T("Romanian"));
		break;
	case 0x0419:
		_stprintf_s(chBuf,1023,_T("Russian"));
		break;
	case 0x041A:
		_stprintf_s(chBuf,1023,_T("Croato-Serbian (Latin)"));
		break;
	case 0x041B:
		_stprintf_s(chBuf,1023,_T("Slovak"));
		break;
	case 0x041C:
		_stprintf_s(chBuf,1023,_T("Albanian"));
		break;
	case 0x041D:
		_stprintf_s(chBuf,1023,_T("Swedish"));
		break;
	case 0x041E:
		_stprintf_s(chBuf,1023,_T("Thai"));
		break;
	case 0x041F:
		_stprintf_s(chBuf,1023,_T("Turkish"));
		break;
	case 0x0420:
		_stprintf_s(chBuf,1023,_T("Urdu"));
		break;
	case 0x0421:
		_stprintf_s(chBuf,1023,_T("Bahasa"));
		break;
	case 0x0804:
		_stprintf_s(chBuf,1023,_T("Simplified Chinese"));
		break;
	case 0x0807:
		_stprintf_s(chBuf,1023,_T("Swiss German"));
		break;
	case 0x0809:
		_stprintf_s(chBuf,1023,_T("U.K. English"));
		break;
	case 0x080A:
		_stprintf_s(chBuf,1023,_T("Mexican Spanish"));
		break;
	case 0x080C:
		_stprintf_s(chBuf,1023,_T("Belgian French"));
		break;
	case 0x0C0C:
		_stprintf_s(chBuf,1023,_T("Canadian French"));
		break;
	case 0x100C:
		_stprintf_s(chBuf,1023,_T("Swiss French"));
		break;
	case 0x0816:
		_stprintf_s(chBuf,1023,_T("Portuguese (Portugal)"));
		break;
	case 0x081A:
		_stprintf_s(chBuf,1023,_T("Serbo-Croatian (Cyrillic)"));
		break;
    default:
        //_stprintf_s(chBuf,1023,_T("Unknown Language"));
		break;
	}

    return chBuf;
}

_tstring CNikPEViewer::GetVerResourceType( DWORD dwResType )
{
    TCHAR chBuf[1024];
    chBuf[0] = 0;

    if(dwResType & VFT_APP)
	{
		_stprintf_s(chBuf,1023,_T("Application"));
	}
	else if(dwResType & VFT_DLL)
	{
		_stprintf_s(chBuf,1023,_T("Dynamic-link library (DLL)"));
	}
	else if(dwResType & VFT_STATIC_LIB)
	{
		_stprintf_s(chBuf,1023,_T("Static-link library (LIB)"));
	}
	else if(dwResType & VFT_DRV)
	{
		_stprintf_s(chBuf,1023,_T("Driver"));
	}
	else if(dwResType & VOS__WINDOWS32)
	{
		_stprintf_s(chBuf,1023,_T("32-bits Windows"));
	}
	else if(dwResType & VFT_FONT)
	{
		_stprintf_s(chBuf,1023,_T("FONT file"));
	}
	else if(dwResType & VFT_VXD)
	{
		_stprintf_s(chBuf,1023,_T("Virtual driver"));
	}
	else
	{
		_stprintf_s(chBuf,1023,_T("UNKNOWN File type"));
	}

    return chBuf;
}

_tstring CNikPEViewer::GetVersionOSName( DWORD dwOScode )
{
    TCHAR chBuf[1024];
    chBuf[0] = 0;

    if(dwOScode & VOS_DOS)
	{
		_stprintf_s(chBuf,1023,_T("MS DOS"));
	}
	else if(dwOScode & VOS_NT)
	{
		_stprintf_s(chBuf,1023,_T("Windows NT"));
	}
	else if(dwOScode & VOS__WINDOWS16)
	{
		_stprintf_s(chBuf,1023,_T("16-bits Windows"));
	}
	else if(dwOScode & VOS__WINDOWS32)
	{
		_stprintf_s(chBuf,1023,_T("32-bits Windows"));
	}
	else if(dwOScode & VOS_OS216)
	{
		_stprintf_s(chBuf,1023,_T("16-bit OS/2"));
	}
	else if(dwOScode & VOS_OS232)
	{
		_stprintf_s(chBuf,1023,_T("32-bit OS/2"));
	}
	else
	{
		_stprintf_s(chBuf,1023,_T("UNKNOWN OS"));
	}

    return chBuf;
}

DWORD CNikPEViewer::GetHexValues(_tstring str)
{
	DWORD dwIDValue = 0;
	if((_T('0') == str.at(0)) 
		&& (_T('x') == str.at(1)))
	{
		str.erase(str.begin(),str.begin()+2);
	}

	int nLength = str.length(),nTemp;
	TCHAR * pBuf = (TCHAR *)str.data();

	for(int i=0;i<nLength;i++)
	{
		if((pBuf[i] >= _T('0')) 
			&& (pBuf[i] <= _T('9')))
		{
			nTemp = pBuf[i] - _T('0');
		}
		else if((pBuf[i] >= _T('A')) 
			&& (pBuf[i] <= _T('F')))
		{
			nTemp = pBuf[i] - _T('A') + 0xA;
		}
		else if((pBuf[i] >= _T('a')) 
			&& (pBuf[i] <= _T('f')))
		{
			nTemp = pBuf[i] - _T('a') + 0xA;
		}
		else
			return 0;
		nTemp <<= (nLength-i-1)*4;
		dwIDValue |= nTemp;
	}

	return dwIDValue;
}

void CNikPEViewer::CreateExecutableInfo( BYTE * pBuf
                                        , DWORD dwSize
                                        , PIMAGE_NT_HEADERS pNTHeader
                                        , PIMAGE_SECTION_HEADER pNTSection)
{
    int i=0;
    DWORD dwTemp = (DWORD)-1;
    int nOffsetBuf = pNTHeader->OptionalHeader.BaseOfCode - pNTHeader->OptionalHeader.SizeOfHeaders;
    DWORD dwEntryPoint = pNTHeader->OptionalHeader.AddressOfEntryPoint;

    m_stExecutableInfo.CleanUp();
	if(pNTHeader->OptionalHeader.Magic == 0x020b)
    {
        //::MessageBox(m_hWnd, _T("Unsupported assembly code!"), _T("Error"), MB_OK);
        return;
    }

    for(i=0; i<pNTHeader->FileHeader.NumberOfSections; i++)
	{
		if((dwEntryPoint >= pNTSection->VirtualAddress)
			&& ((dwEntryPoint <= (pNTSection->VirtualAddress+pNTSection->SizeOfRawData))))
		{
			dwTemp = pNTSection->VirtualAddress - pNTSection->PointerToRawData;
			break;
		}
		pNTSection++;
	}

    if(i >= pNTHeader->FileHeader.NumberOfSections)
        return;

    if((int)dwTemp != -1)
		nOffsetBuf = dwTemp;

    if(nOffsetBuf < 0)
        return;

    dwTemp = pNTSection->VirtualAddress - nOffsetBuf;
    if(dwTemp < 0)
        return;
    if((dwTemp+pNTSection->SizeOfRawData) > dwSize)
        return;

    m_stExecutableInfo.m_pBuffer        = pBuf;
    m_stExecutableInfo.m_dwBufferSize   = dwSize;
    m_stExecutableInfo.m_dwStartOffset  = dwTemp;
    m_stExecutableInfo.m_dwEndOffset    = dwTemp + pNTSection->SizeOfRawData;
    m_stExecutableInfo.m_dwEntryPoint   = pNTHeader->OptionalHeader.AddressOfEntryPoint - nOffsetBuf;
    m_stExecutableInfo.m_stNTHeader     = *pNTHeader;
}

/*void CNikPEViewer::CreateImportFunctionList( BYTE * pBuf
                                            , DWORD dwOffset
							                , DWORD dwMinus
							                , int nSize
							                , PIMAGE_NT_HEADERS pNTHeader
							                , PIMAGE_SECTION_HEADER pNTSection)
{
	m_vImportDllList.clear();

	DWORD dwTemp = (DWORD)-1;

	for(int i=0;i<pNTHeader->FileHeader.NumberOfSections;i++)
	{
		if((pNTHeader->OptionalHeader.DataDirectory[1].VirtualAddress 
													>= pNTSection->VirtualAddress)
			&& ((pNTHeader->OptionalHeader.DataDirectory[1].VirtualAddress+pNTHeader->OptionalHeader.DataDirectory[1].Size 
													<= pNTSection->VirtualAddress+pNTSection->SizeOfRawData)))
		{
			dwTemp = pNTSection->VirtualAddress - pNTSection->PointerToRawData;
			break;
		}
		pNTSection++;
	}

	if(dwTemp != -1)
		dwMinus = dwTemp;
	else
	{
		//if((nSize+pNTHeader->OptionalHeader.DataDirectory[1].Size) 
		//	< pNTHeader->OptionalHeader.DataDirectory[1].VirtualAddress)
			return;
	}

    m_nDiffOffset = (int)dwMinus;

	TCHAR chFunction[1024];

	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)&pBuf[dwOffset - dwMinus];
	IMAGE_THUNK_DATA32 * pImageThunk = NULL;

	while(pImport->OriginalFirstThunk)
	{
		IMPORTDLLLIST stImportList;
		wsprintf(chFunction,_T("%S"),(TCHAR *)&pBuf[pImport->Name - dwMinus]);
		stImportList.m_strDllName = chFunction;
		memcpy(&stImportList.m_stImageDescriptor, pImport, sizeof(IMAGE_IMPORT_DESCRIPTOR));

		pImageThunk = (IMAGE_THUNK_DATA32 *)&pBuf[pImport->OriginalFirstThunk - dwMinus];

		while(pImageThunk->u1.Ordinal)
		{
			IMPORTFUNCTIONLIST stImportFunction;

			DWORD dwOffset = (0x7FFFFFFF & pImageThunk->u1.Function);

			int nOffset = dwOffset - dwMinus;
			if((nOffset > 0) 
				&& (nOffset < nSize))
			{
				short * pOrdinal = (short *)&pBuf[nOffset];
				stImportFunction.m_nOrdinalNo = pOrdinal[0];
				pOrdinal++;
				if(!(0x80000000 & pImageThunk->u1.Function))
				{
					wsprintf(chFunction,_T("%S"),(TCHAR *)(pOrdinal));
					stImportFunction.m_strFunctionName = chFunction;
				}
			}
			stImportList.m_vImportFunctionList.push_back(stImportFunction);
			pImageThunk++;
		}

		m_vImportDllList.push_back( stImportList );
        m_stExecutableInfo.m_vImportDllList.push_back( stImportList );
		pImport++;
	}
}

void CNikPEViewer::CreateExportFunctionList( BYTE * pBuf
							                , DWORD dwOffset
							                , DWORD dwMinus
							                , int nSize
							                , PIMAGE_NT_HEADERS pNTHeader
							                , PIMAGE_SECTION_HEADER pNTSection)
{
	m_vExportFunctionList.clear();

	DWORD dwTemp = (DWORD)-1;

	for(int i=0; i<pNTHeader->FileHeader.NumberOfSections; i++)
	{
		if((pNTHeader->OptionalHeader.DataDirectory[0].VirtualAddress 
													>= pNTSection->VirtualAddress)
			&& ((pNTHeader->OptionalHeader.DataDirectory[0].VirtualAddress+pNTHeader->OptionalHeader.DataDirectory[1].Size 
													<= pNTSection->VirtualAddress+pNTSection->SizeOfRawData)))
		{
			dwTemp = pNTSection->VirtualAddress - pNTSection->PointerToRawData;
			break;
		}
		pNTSection++;
	}

	if(dwTemp != -1)
		dwMinus = dwTemp;
	else
	{
		//if((nSize+pNTHeader->OptionalHeader.DataDirectory[0].Size) 
		//	< pNTHeader->OptionalHeader.DataDirectory[0].VirtualAddress)
			return;
	}

    m_nDiffOffset = (int)dwMinus;

	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)&pBuf[dwOffset - dwMinus];
	int nCount = pExport->NumberOfFunctions;
	short * pOrdinal = (short *)&pBuf[pExport->AddressOfNameOrdinals - dwMinus];
	DWORD * pstrFunctionName = (DWORD *)&pBuf[pExport->AddressOfNames - dwMinus];
	DWORD * pdwAdd = (DWORD *)&pBuf[pExport->AddressOfFunctions - dwMinus];
	TCHAR chFunction[1024];
	int nNames = pExport->NumberOfNames,j = 0;

	for(int i=0;i<nCount;i++)
	{
		EXPORTFUNCTIONLIST stExportFunction;
		stExportFunction.m_dwOrdianlNo = pOrdinal[0] + pExport->Base;
		stExportFunction.m_dwHint = i;
		int nOffset = (int)(pstrFunctionName[0] - dwMinus);
		if((j < nNames) 
			&& (nOffset > 0) 
			&& (nOffset < nSize))
		{
			wsprintf(chFunction,_T("%S"),(TCHAR *)&pBuf[nOffset]);
			j++;
			stExportFunction.m_strFunctionName = chFunction;
		}
		else
		{
			chFunction[0] = 0;
			break;
		}

		stExportFunction.m_dwEntryPoint = (DWORD)pdwAdd[1];

		m_vExportFunctionList.push_back( stExportFunction );
        m_stExecutableInfo.m_vExportFunctionList.push_back( stExportFunction );

		pdwAdd++;
		pOrdinal++;
		pstrFunctionName++;
	}
}*/

void CNikPEViewer::CreateImportFunctionList( BYTE * pBuf
                                            , DWORD dwOffset
							                , DWORD dwMinus
							                , int nSize
							                , PIMAGE_NT_HEADERS pNTHeader
							                , PIMAGE_SECTION_HEADER pNTSection)
{
	m_vImportDllList.clear();

	int nExtended = 0;
	PIMAGE_NT_HEADERS32 pNTHeader32 = (PIMAGE_NT_HEADERS32)pNTHeader;
	PIMAGE_NT_HEADERS64 pNTHeader64 = (PIMAGE_NT_HEADERS64)pNTHeader;

	DWORD dwTemp = (DWORD)-1;

	if(pNTHeader32->OptionalHeader.Magic == 0x020b)
		nExtended = 1;

	if(0 == nExtended)
	{
		for(int i=0;i<pNTHeader32->FileHeader.NumberOfSections;i++)
		{
			if((pNTHeader32->OptionalHeader.DataDirectory[1].VirtualAddress 
														>= pNTSection->VirtualAddress)
				&& ((pNTHeader32->OptionalHeader.DataDirectory[1].VirtualAddress+pNTHeader32->OptionalHeader.DataDirectory[1].Size 
														<= pNTSection->VirtualAddress+pNTSection->SizeOfRawData)))
			{
				dwTemp = pNTSection->VirtualAddress - pNTSection->PointerToRawData;
				break;
			}
			pNTSection++;
		}

		if(dwTemp != -1)			//its crashing here!
			dwMinus = dwTemp;
		else
		{
			if((nSize+pNTHeader32->OptionalHeader.DataDirectory[1].Size) 
				< pNTHeader32->OptionalHeader.DataDirectory[1].VirtualAddress)
				return;
		}
	}
	else
	{
		for(int i=0;i<pNTHeader64->FileHeader.NumberOfSections;i++)
		{
			if((pNTHeader64->OptionalHeader.DataDirectory[1].VirtualAddress 
														>= pNTSection->VirtualAddress)
				&& ((pNTHeader64->OptionalHeader.DataDirectory[1].VirtualAddress+pNTHeader64->OptionalHeader.DataDirectory[1].Size 
														<= pNTSection->VirtualAddress+pNTSection->SizeOfRawData)))
			{
				dwTemp = pNTSection->VirtualAddress - pNTSection->PointerToRawData;
				break;
			}
			pNTSection++;
		}

		if(dwTemp != -1)			//its crashing here!
			dwMinus = dwTemp;
		else
		{
			if((nSize+pNTHeader64->OptionalHeader.DataDirectory[1].Size) 
				< pNTHeader64->OptionalHeader.DataDirectory[1].VirtualAddress)
				return;
		}
	}

    m_nDiffOffset = (int)dwMinus;

	TCHAR chFunction[1024];

	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)&pBuf[dwOffset - dwMinus];
	IMAGE_THUNK_DATA32 * pImageThunk = NULL;

    while((pImport->OriginalFirstThunk) || (pImport->FirstThunk))
	{
		IMPORTDLLLIST stImportList;
		wsprintf(chFunction,_T("%S"),(TCHAR *)&pBuf[pImport->Name - dwMinus]);
		stImportList.m_strDllName = chFunction;
		memcpy(&stImportList.m_stImageDescriptor, pImport, sizeof(IMAGE_IMPORT_DESCRIPTOR));

        if(pImport->OriginalFirstThunk)
		    pImageThunk = (IMAGE_THUNK_DATA32 *)&pBuf[pImport->OriginalFirstThunk - dwMinus];
        else
            pImageThunk = (IMAGE_THUNK_DATA32 *)&pBuf[pImport->FirstThunk - dwMinus];

		while(pImageThunk->u1.Ordinal)
		{
			IMPORTFUNCTIONLIST stImportFunction;

			DWORD dwOffset = (0x7FFFFFFF & pImageThunk->u1.Function);

			int nOffset = dwOffset - dwMinus;
			if((nOffset > 0) 
				&& (nOffset < nSize))
			{
				short * pOrdinal = (short *)&pBuf[nOffset];
				stImportFunction.m_nOrdinalNo = pOrdinal[0];
				pOrdinal++;
				if(!(0x80000000 & pImageThunk->u1.Function))
				{
					wsprintf(chFunction,_T("%S"),(TCHAR *)(pOrdinal));
					stImportFunction.m_strFunctionName = chFunction;
				}
			}
			stImportList.m_vImportFunctionList.push_back(stImportFunction);
			pImageThunk++;
		}

		m_vImportDllList.push_back(stImportList);
        m_stExecutableInfo.m_vImportDllList.push_back( stImportList );
		pImport++;
	}
}

void CNikPEViewer::CreateExportFunctionList( BYTE * pBuf
							                , DWORD dwOffset
							                , DWORD dwMinus
							                , int nSize
							                , PIMAGE_NT_HEADERS pNTHeader
							                , PIMAGE_SECTION_HEADER pNTSection)
{
	m_vExportFunctionList.clear();

	int nExtended = 0;
	PIMAGE_NT_HEADERS32 pNTHeader32 = (PIMAGE_NT_HEADERS32)pNTHeader;
	PIMAGE_NT_HEADERS64 pNTHeader64 = (PIMAGE_NT_HEADERS64)pNTHeader;

	DWORD dwTemp = (DWORD)-1;

	if(pNTHeader32->OptionalHeader.Magic == 0x020b)
		nExtended = 1;

	if(0 == nExtended)
	{
		for(int i=0; i<pNTHeader32->FileHeader.NumberOfSections; i++)
		{
			if((pNTHeader32->OptionalHeader.DataDirectory[0].VirtualAddress 
														>= pNTSection->VirtualAddress)
				&& ((pNTHeader32->OptionalHeader.DataDirectory[0].VirtualAddress+pNTHeader32->OptionalHeader.DataDirectory[1].Size 
														<= pNTSection->VirtualAddress+pNTSection->SizeOfRawData)))
			{
				dwTemp = pNTSection->VirtualAddress - pNTSection->PointerToRawData;
				break;
			}
			pNTSection++;
		}

		if(dwTemp != -1)
			dwMinus = dwTemp;
		else
		{
			if((nSize+pNTHeader32->OptionalHeader.DataDirectory[0].Size) 
				< pNTHeader32->OptionalHeader.DataDirectory[0].VirtualAddress)
				return;
		}
	}
	else
	{
		for(int i=0; i<pNTHeader64->FileHeader.NumberOfSections; i++)
		{
			if((pNTHeader64->OptionalHeader.DataDirectory[0].VirtualAddress 
														>= pNTSection->VirtualAddress)
				&& ((pNTHeader64->OptionalHeader.DataDirectory[0].VirtualAddress+pNTHeader64->OptionalHeader.DataDirectory[1].Size 
														<= pNTSection->VirtualAddress+pNTSection->SizeOfRawData)))
			{
				dwTemp = pNTSection->VirtualAddress - pNTSection->PointerToRawData;
				break;
			}
			pNTSection++;
		}

		if(dwTemp != -1)
			dwMinus = dwTemp;
		else
		{
			if((nSize+pNTHeader64->OptionalHeader.DataDirectory[0].Size) 
				< pNTHeader64->OptionalHeader.DataDirectory[0].VirtualAddress)
				return;
		}
	}

    m_nDiffOffset = (int)dwMinus;

	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)&pBuf[dwOffset - dwMinus];
	int nCount = pExport->NumberOfFunctions;
	short * pOrdinal = (short *)&pBuf[pExport->AddressOfNameOrdinals - dwMinus];
	DWORD * pstrFunctionName = (DWORD *)&pBuf[pExport->AddressOfNames - dwMinus];
	DWORD * pdwAdd = (DWORD *)&pBuf[pExport->AddressOfFunctions - dwMinus];
	TCHAR chFunction[1024];
	int nNames = pExport->NumberOfNames,j = 0;

	for(int i=0;i<nCount;i++)
	{
		EXPORTFUNCTIONLIST stExportFunction;
		stExportFunction.m_dwOrdianlNo = pOrdinal[0] + pExport->Base;
		stExportFunction.m_dwHint = i;
		int nOffset = (int)(pstrFunctionName[0] - dwMinus);
		if((j < nNames) 
			&& (nOffset > 0) 
			&& (nOffset < nSize))
		{
			wsprintf(chFunction,_T("%S"),(TCHAR *)&pBuf[nOffset]);
			j++;
			stExportFunction.m_strFunctionName = chFunction;
		}
		else
		{
			chFunction[0] = 0;
			break;
		}

		stExportFunction.m_dwEntryPoint = (DWORD)pdwAdd[1];

		m_vExportFunctionList.push_back(stExportFunction);
        m_stExecutableInfo.m_vExportFunctionList.push_back( stExportFunction );

		pdwAdd++;
		pOrdinal++;
		pstrFunctionName++;
	}
}

void CNikPEViewer::CreateResourceList( BYTE * pBuffer
                                      , int nSize)
{
	m_dwResourceBaseAdd = (DWORD)pBuffer;

	if(m_pstResourceList)
	{
		delete m_pstResourceList;
		m_pstResourceList = NULL;
	}

	m_pstResourceList = DecodeResourceItems(pBuffer, nSize, 0);
}

void CNikPEViewer::GetResourceName( DWORD nID
                                   , TCHAR * str)
{
	if(nID & IMAGE_RESOURCE_NAME_IS_STRING)
	{
		nID &= 0x7FFFFFFF;
		PIMAGE_RESOURCE_DIR_STRING_U prdsu = (PIMAGE_RESOURCE_DIR_STRING_U)(m_dwResourceBaseAdd + nID);
		WCHAR * wchStr = new WCHAR [prdsu->Length+1];
		if(wchStr)
		{
			wchStr[prdsu->Length] = 0;
			memcpy(wchStr, prdsu->NameString, (prdsu->Length)*sizeof(WCHAR));

#ifdef  _UNICODE
			str[0] = 0;
			_stprintf_s(str, prdsu->Length+1, _T("%s"), wchStr);
#else
			_stprintf_s(str, prdsu->Length+1, _T("%S"), wchStr);
#endif
			delete [] wchStr;
		}
	}
}

RESOURCELIST * CNikPEViewer::DecodeResourceItems( BYTE * pBuffer
                                                 , int nFileSize
                                                 , int nNo)
{
	PIMAGE_RESOURCE_DIRECTORY pResourDir = (PIMAGE_RESOURCE_DIRECTORY)pBuffer;
	int j;

	RESOURCELIST * pListResource = new RESOURCELIST;

	pListResource->m_nType = pResourDir->Characteristics;
	pListResource->m_bIsDir = TRUE;

	pBuffer += sizeof(IMAGE_RESOURCE_DIRECTORY);

	for(j=0; j<pResourDir->NumberOfNamedEntries; j++)
	{
		DecodeDirResourceEntry( pBuffer, m_dwResourceBaseAdd, nFileSize, nNo+1, pListResource);
		pBuffer += sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY);
	}

	for(j=0; j<pResourDir->NumberOfIdEntries; j++)
	{
		DecodeDirResourceEntry( pBuffer, m_dwResourceBaseAdd, nFileSize, nNo+1, pListResource);
		pBuffer += sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY);
	}
	return pListResource;
}

void CNikPEViewer::DecodeDirResourceEntry( BYTE * pBuffer
                                          , int nBaseAddress
                                          , int nFileSize
                                          , int nNo
                                          , RESOURCELIST * pResource)
{
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pResourceDirEnteries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)pBuffer;
	PIMAGE_RESOURCE_DATA_ENTRY pResourceDataEntry;

	if(pResourceDirEnteries->OffsetToData  
		& IMAGE_RESOURCE_DATA_IS_DIRECTORY)
	{
		RESOURCELIST * pListResource = DecodeResourceItems((BYTE *)(nBaseAddress + (pResourceDirEnteries->OffsetToData & 0x7FFFFFFF)) 
														, nFileSize
														, nNo+1);		
		pResource->m_vDirList.push_back( pListResource );

		if(1 == nNo && (pResourceDirEnteries->Id < 18))
		{
			pListResource->m_nType = pResourceDirEnteries->Id;
			_tcscpy_s(pListResource->m_chName, 128, g_chResourceTypes[pResourceDirEnteries->Id]);
			for(int i=0; i<(int)pListResource->m_vDirList.size(); i++)
			{
				RESOURCELIST * pListR = pListResource->m_vDirList.at(i);
				if(pListR)
					pListR->m_nType = pListResource->m_nType;
			}
		}
		else
		{
			if(pResourceDirEnteries->NameIsString)
			{
				GetResourceName(pResourceDirEnteries->Name, pListResource->m_chName );
			}
			else
			{
				pListResource->m_chName[0] = 0;
				_stprintf_s( pListResource->m_chName, 128, _T("(0x%X)"), pResourceDirEnteries->Id );
			}
		}
		return;
	}

	pResourceDataEntry = (PIMAGE_RESOURCE_DATA_ENTRY)(nBaseAddress + pResourceDirEnteries->OffsetToData);
	pResource->m_bIsDir = FALSE;
	pResource->m_nOffsetToData = pResourceDataEntry->OffsetToData - m_dwDiffAddress;
	pResource->m_nSize = pResourceDataEntry->Size;
}

int CNikPEViewer::DecodePEFile(_tstring strFileName)
{
    m_vImageSectionHeaderList.clear();
    memset(&m_stImageDosHeader, 0, sizeof(IMAGE_DOS_HEADER));
    memset(&m_stImageNtHeaders32, 0, sizeof(IMAGE_DOS_HEADER));
	memset(&m_stImageNtHeaders64, 0, sizeof(IMAGE_DOS_HEADER));

	BYTE * pBuffer = NULL;
	HANDLE hFile = CreateFile(strFileName.data(), GENERIC_READ, FILE_SHARE_READ, NULL,
							OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);					
	if ( hFile == INVALID_HANDLE_VALUE )
		return -1;

	DWORD dwFileSize = 0;
	dwFileSize = GetFileSize(hFile,NULL);
	if(dwFileSize)
	{
		pBuffer = new BYTE [dwFileSize + 2];
		if(pBuffer)
		{
			pBuffer[dwFileSize] = 0;
			pBuffer[dwFileSize+1] = 0;
			DWORD dwRead = 0;
			ReadFile(hFile,pBuffer,dwFileSize,&dwRead,NULL);
			if(dwRead == dwFileSize)
			{
				PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)pBuffer;
				if ( dosHeader->e_magic == IMAGE_DOS_SIGNATURE )
				{
                    memcpy(&m_stImageDosHeader, dosHeader, sizeof(IMAGE_DOS_HEADER));
					PIMAGE_NT_HEADERS32 pNTHeader32 = (PIMAGE_NT_HEADERS32)(pBuffer + dosHeader->e_lfanew);
					if(pNTHeader32->Signature == IMAGE_NT_SIGNATURE)
					{
						PIMAGE_SECTION_HEADER pTemppNTSection;

						PIMAGE_SECTION_HEADER pNTSection;
						int nOffsetBuf = pNTHeader32->OptionalHeader.BaseOfCode - pNTHeader32->OptionalHeader.SizeOfHeaders;
						memcpy(&m_stImageNtHeaders32, pNTHeader32, sizeof(IMAGE_NT_HEADERS32));

						if(pNTHeader32->OptionalHeader.Magic == 0x010b)
						{
							pNTSection = (PIMAGE_SECTION_HEADER)(pNTHeader32+1);

                            CreateExecutableInfo(pBuffer, dwFileSize, pNTHeader32, pNTSection);

							if(pNTHeader32->OptionalHeader.DataDirectory[0].Size)	//	Export directory table
							{
								CreateExportFunctionList(pBuffer, pNTHeader32->OptionalHeader.DataDirectory[0].VirtualAddress
											, nOffsetBuf, dwFileSize, pNTHeader32, pNTSection);
							}

							if(pNTHeader32->OptionalHeader.DataDirectory[1].Size)	//	Import directory table
							{
								CreateImportFunctionList(pBuffer, pNTHeader32->OptionalHeader.DataDirectory[1].VirtualAddress
											, nOffsetBuf, dwFileSize, pNTHeader32, pNTSection);
							}

							pTemppNTSection = pNTSection;

							if(pNTHeader32->OptionalHeader.DataDirectory[2].Size)	//  Resource items table
							{
								for(int i=0; i<pNTHeader32->FileHeader.NumberOfSections; i++)
								{
									if(pNTHeader32->OptionalHeader.DataDirectory[2].VirtualAddress 
																				== pTemppNTSection->VirtualAddress)
									{
										m_dwDiffAddress = pTemppNTSection->VirtualAddress - pTemppNTSection->PointerToRawData;
										CreateResourceList((BYTE *)&pBuffer[pTemppNTSection->PointerToRawData]
														, pTemppNTSection->SizeOfRawData);
										break;
									}
									pTemppNTSection++;
								}
							}

							pTemppNTSection = pNTSection;

							for(int i=0; i<pNTHeader32->FileHeader.NumberOfSections; i++)
							{
								pTemppNTSection->Name[7] = 0;
								m_vImageSectionHeaderList.push_back(*pTemppNTSection);
								pTemppNTSection++;
							}
						}
						else if(pNTHeader32->OptionalHeader.Magic == 0x020b)
						{
							PIMAGE_NT_HEADERS64 pNTHeader64 = (PIMAGE_NT_HEADERS64)(pBuffer + dosHeader->e_lfanew);
							memcpy(&m_stImageNtHeaders64, pNTHeader64, sizeof(IMAGE_NT_HEADERS64));

							pNTSection = (PIMAGE_SECTION_HEADER)(pNTHeader64+1);

                            CreateExecutableInfo(pBuffer, dwFileSize, (PIMAGE_NT_HEADERS)pNTHeader64, pNTSection);

							if(pNTHeader64->OptionalHeader.DataDirectory[0].Size)	//	Export directory table
							{
								CreateExportFunctionList(pBuffer, pNTHeader64->OptionalHeader.DataDirectory[0].VirtualAddress
											, nOffsetBuf, dwFileSize, (PIMAGE_NT_HEADERS)pNTHeader64, pNTSection);
							}

							if(pNTHeader64->OptionalHeader.DataDirectory[1].Size)	//	Import directory table
							{
								CreateImportFunctionList(pBuffer, pNTHeader64->OptionalHeader.DataDirectory[1].VirtualAddress
											, nOffsetBuf, dwFileSize, (PIMAGE_NT_HEADERS)pNTHeader64, pNTSection);
							}

							pTemppNTSection = pNTSection;

							if(pNTHeader64->OptionalHeader.DataDirectory[2].Size)	//  Resource items table
							{
								for(int i=0; i<pNTHeader32->FileHeader.NumberOfSections; i++)
								{
									if(pNTHeader64->OptionalHeader.DataDirectory[2].VirtualAddress 
																				== pTemppNTSection->VirtualAddress)
									{
										m_dwDiffAddress = pTemppNTSection->VirtualAddress - pTemppNTSection->PointerToRawData;
										CreateResourceList((BYTE *)&pBuffer[pTemppNTSection->PointerToRawData]
														, pTemppNTSection->SizeOfRawData);
										break;
									}
									pTemppNTSection++;
								}
							}

							pTemppNTSection = pNTSection;

							for(int i=0; i<pNTHeader64->FileHeader.NumberOfSections; i++)
							{
								pTemppNTSection->Name[7] = 0;
								m_vImageSectionHeaderList.push_back(*pTemppNTSection);
								pTemppNTSection++;
							}
						}                        
					}
					else
					{
						int nRetCode = -6;

						if(pNTHeader32->FileHeader.Machine == 0x014c)
							nRetCode = -4;
						else if(( dosHeader->e_magic == IMAGE_DOS_SIGNATURE ) 
								&& (pNTHeader32->FileHeader.Machine != 0x014c))
							nRetCode = -5;

						delete [] pBuffer;
						pBuffer = NULL;

                        return nRetCode;
					}
				}
				else
				{
					delete [] pBuffer;
					pBuffer = NULL;
                    return -3;
				}
			}
            else
                return -2;
		}
	}

    CloseHandle(hFile);

    m_pPEBuffer = pBuffer;
    m_strFileName = strFileName;
    m_dwPEFileSize = dwFileSize;

    return 0;
}

int CNikPEViewer::SetAllViewsDefaultPos()
{
    if(m_hWnd)
    {
        RECT rc;
        GetClientRect(GetWindowHandle(), &rc);
        int nHeight = rc.bottom - rc.top;
        int nWidth = rc.right - rc.left;

        int nWindow = 5;

        SetWindowPos( m_cPEVTreeCtrl.GetWindowHandle(), NULL, rc.left, rc.top
                    , nWidth/nWindow, nHeight, SWP_SHOWWINDOW );

        rc.left += nWidth/nWindow;
        SetWindowPos( m_cPEVHeaderList.GetWindowHandle(), NULL, rc.left, rc.top
                    , nWidth/nWindow, nHeight, SWP_SHOWWINDOW );

        rc.left += nWidth/nWindow;
        SetWindowPos( m_cDataSectionList.GetWindowHandle(), NULL, rc.left, rc.top
                    , nWidth/nWindow, nHeight, SWP_SHOWWINDOW );

        //if(GetImportFunctionList().size() > 0)
        {
            rc.left += nWidth/nWindow;
            SetWindowPos( m_cPEVImportList.GetWindowHandle(), NULL, rc.left, rc.top
                    , nWidth/nWindow, nHeight, SWP_SHOWWINDOW );
        }

        //if(GetExportFunctionList().size() > 0)
        {
            rc.left += nWidth/nWindow;
            SetWindowPos( m_cPEVExportList.GetWindowHandle(), NULL, rc.left, rc.top
                    , nWidth/nWindow, nHeight, SWP_SHOWWINDOW );
        }

        SetWindowPos( m_cDisAssemblyList.GetWindowHandle(), HWND_TOP, (nWidth/5)*2, 22
                    , ((nWidth/nWindow) * 3), nHeight/2, SWP_SHOWWINDOW );

        SetWindowPos( m_cPEBinaryViewer.GetWindowHandle(), HWND_TOP, (nWidth/5)*2, nHeight/2
                    , ((nWidth/nWindow) * 3), nHeight/2, SWP_SHOWWINDOW );
    }

    return 0;
}

int CNikPEViewer::InitPEFrameWindow()
{
    if(m_hWnd)
    {
        RECT rc;
        GetClientRect(GetWindowHandle(), &rc);
        int nHeight = rc.bottom - rc.top;
        int nWidth = rc.right - rc.left;

        int nWindow = 5;

        RECT rc_1 = rc;
        rc_1.left = (nWidth/nWindow)*2;
        rc_1.right = rc_1.left + ((nWidth/nWindow) * 3);
        rc_1.top = nHeight/2;
        rc_1.bottom = rc_1.top + nHeight/2;
        m_cPEBinaryViewer.CreateBinaryViewer(m_hInstance, m_hWnd, rc_1, _T("Module binary view"));
        m_cPEBinaryViewer.SetBinaryData(m_pPEBuffer, m_dwPEFileSize);
        
        rc.right = rc.left + nWidth/nWindow;
        m_hTreeCtrl = m_cPEVTreeCtrl.CreatePETreeCtrl( m_hInstance, m_hWnd, rc);

        rc.left = rc.right;
        rc.right = rc.left + nWidth/nWindow;
        m_cPEVHeaderList.CreatePEListViewCtrl(m_hInstance, m_hWnd, rc, _T("PE Headers"));
        m_cPEVHeaderList.FillPEVListViewInfo( this );

        rc.left = rc.right;
        rc.right = rc.left + nWidth/nWindow;
        m_cDataSectionList.CreatePEListViewCtrl(m_hInstance, m_hWnd, rc, _T("Sections & Data Directories"));
        m_cDataSectionList.FillPEVListViewInfo( this );

        //if(GetImportFunctionList().size() > 0)
        {
            rc.left = rc.right;
            rc.right = rc.left + nWidth/nWindow;
            m_cPEVImportList.CreatePEListViewCtrl(m_hInstance, m_hWnd, rc, _T("Import Functions"));
            m_cPEVImportList.FillPEVListViewInfo( this );
        }

        //if(GetExportFunctionList().size() > 0)
        {
            rc.left = rc.right;
            rc.right = rc.left + nWidth/nWindow;
            m_cPEVExportList.CreatePEListViewCtrl(m_hInstance, m_hWnd, rc, _T("Export Functions"));
            m_cPEVExportList.FillPEVListViewInfo( this );
        }
    }
    else
        return -1;

    return 0;
}

int CNikPEViewer::ShowPEHtmlView()
{
    if(m_hWnd 
        && (!m_cNikHTMLWindow.GetWindowHandle()))
    {
        RECT rc, rc1;
        GetClientRect(GetWindowHandle(), &rc);
        int nHeight = rc.bottom - rc.top;
        int nWidth = rc.right - rc.left;

        rc1.left = rc.left + nWidth/16;
        rc1.right = rc.left + ((nWidth*15)/16);
        rc1.top = rc.top + nHeight/16;
        rc1.bottom = rc.top + ((nHeight*15)/16);
        
        m_cNikHTMLWindow.CreateHTMLWindow(m_hInstance, m_hWnd, rc1, _T("PE HTML view"));
        m_cNikHTMLWindow.CreateEmbeddedWebControl( this );
    }
    else
    {
        SetWindowPos( m_cNikHTMLWindow.GetWindowHandle(), HWND_TOP
            , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
    }
    
    return 0;
}

HWND CNikPEViewer::CreatePEFrameWindow(HINSTANCE hInstance
                                       , HWND hParentWnd)
{
    RegisterNikWindows(hInstance);

    _tstring strTemp;
    int nPos = m_strFileName.rfind(_T('\\'));
    if(nPos >= 0)
    {
        nPos++;
        strTemp = m_strFileName.substr(nPos, m_strFileName.length());
    }
    else
        strTemp = m_strFileName;

    m_strShortName = strTemp;

    MDICREATESTRUCT mcs;
    memset(&mcs, 0, sizeof(mcs));
    mcs.szTitle = m_strShortName.c_str();
    mcs.szClass = g_szNikPEViewerClass;
    mcs.hOwner  = m_hInstance;
    mcs.x = mcs.cx = CW_USEDEFAULT; 
    mcs.y = mcs.cy = CW_USEDEFAULT;
    mcs.style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;

    m_hWnd = (HWND) SendMessage (hParentWnd, WM_MDICREATE, 0, (LONG) (LPMDICREATESTRUCT) &mcs);

    if(m_hWnd)
    {
        g_pNikPEViewer = this;
        SetWindowLong( m_hWnd, GWL_USERDATA, (LONG)g_pNikPEViewer);

        ShowWindow(m_hWnd, SW_MAXIMIZE);
    }

    return m_hWnd;
}

int CNikPEViewer::SavePEHtmlFilePart(_tstring strHtmlText
                                 , _tstring strFilePath
                                 , _tstring strFileName)
{
    _tstring strTempFile = strFilePath + strFileName;

    HANDLE hFile = CreateFile( strTempFile.data()
                               , GENERIC_WRITE | GENERIC_READ
                               , 0, NULL, CREATE_ALWAYS
                               , FILE_ATTRIBUTE_NORMAL
                               , NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return -1;

    DWORD dwBytesWritten, dwBufSize = strHtmlText.length() * sizeof(TCHAR);
    WriteFile( hFile, strHtmlText.data(), dwBufSize, &dwBytesWritten, NULL);

    CloseHandle( hFile );

    return 0;
}

int CNikPEViewer::SavePEFileRecords(_tstring strFileName)
{
    if(!strFileName.empty())
    {
        TCHAR chPathBuf[1024];
        m_strSaveFileName = strFileName;

        _tstring strTempPath;
        GetTempPath( 1023, chPathBuf);

        int iPos = strFileName.find_last_of(_T('\\'),strFileName.length());
        strTempPath = strFileName.substr(0, iPos);

        //strTempPath = chPathBuf;
        strTempPath += _T("\\nik_pedata");
        CreateDirectory(strTempPath.data(), NULL);

        SavePEHtmlFilePart( m_cNikHTMLWindow.SetTreeItemStr(this), strTempPath, _T("\\petree.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.CreatePEDosHrHtmlPageStr(), strTempPath, _T("\\pedoshr.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.CreatePENTHtmlPageString(), strTempPath, _T("\\penthr.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.CreateDataHtmlPageString(), strTempPath, _T("\\pedata.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.CreateSectionHtmlPageString(), strTempPath, _T("\\pesect.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.CreateImportHtmlPageString(), strTempPath, _T("\\peimport.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.CreateExportHtmlPageString(), strTempPath, _T("\\peexport.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.GetHtmlStrVersion(this), strTempPath, _T("\\peversion.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.GetHtmlStrAccelarator(this), strTempPath, _T("\\peaccel.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.GetHtmlResStringTable(this), strTempPath, _T("\\pestring.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.GetHtmlResMenuItems(this), strTempPath, _T("\\pemenu.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.GetHtmlResBitmap(this,_T("BITMAP"),(TCHAR *)strTempPath.data())
                                , strTempPath, _T("\\pebitmap.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.GetHtmlResBitmap(this,_T("ICON"),(TCHAR *)strTempPath.data())
                                , strTempPath, _T("\\peicon.html"));
        Sleep(10);
        SavePEHtmlFilePart( m_cNikHTMLWindow.GetHtmlResBitmap(this,_T("CURSOR"),(TCHAR *)strTempPath.data())
                                , strTempPath, _T("\\pecursor.html"));
        Sleep(10);

        _tstring strTempHtml;
        strTempHtml += _T("<html>");
        strTempHtml += _T("<head>");
        strTempHtml += _T("<title>PE NikViewer v1.0</title>");
        strTempHtml += _T("</head>");
        strTempHtml += _T("<frameset cols=\"25%,75%\">");
        strTempHtml += _T("<frame src=\"");
        strTempHtml += strTempPath;
        strTempHtml += _T("\\petree.html");
        strTempHtml += _T("\" />");
        strTempHtml += _T("<frame src=\"");
        strTempHtml += strTempPath;
        strTempHtml += _T("\\penthr.html");
        strTempHtml += _T("\" name=\"main\" />");
        strTempHtml += _T("</frameset>");
        strTempHtml += _T("</html>");

        SavePEHtmlFilePart( strTempHtml.data(), _T(""), m_strSaveFileName);

        //ShellExecute(NULL, _T("open"), m_strSaveFileName.data(), NULL, NULL, SW_SHOWNORMAL);
    }

    return 0;
}

_tstring CNikPEViewer::SaveFileDialogBox(HWND hWnd)
{
    TCHAR strFile[_MAX_PATH];
    strFile[0] = 0;

    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = strFile;
    ofn.nMaxFile = _MAX_PATH;
    ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
    ofn.lpstrFilter = _T("Web page\0*.html;*.htm;\0All files\0*.*\0\0");
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrDefExt = _T("Exe");

    if(GetSaveFileName(&ofn))
        return ofn.lpstrFile;

    return ofn.lpstrFile;
}

_tstring CNikPEViewer::OpenFileDialogBox(HWND hWnd)
{
    TCHAR strFile[_MAX_PATH];
    strFile[0] = 0;

    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = strFile;
    ofn.nMaxFile = _MAX_PATH;
    ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
    ofn.lpstrFilter = _T("PE files\0*.exe;*.dll;*.sys;*.obj\0All files\0*.*\0\0");
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrDefExt = _T("Exe");

    if(GetOpenFileName(&ofn))
        return ofn.lpstrFile;

    return ofn.lpstrFile;
}

LRESULT CALLBACK CNikPEViewer::PEViewerWndProc( HWND hWnd
                                               , UINT uMsg
                                               , WPARAM wParam
                                               , LPARAM lParam )
{
    switch( uMsg )
    {
    case WM_SIZE:
        {
        }
        return DefMDIChildProc( hWnd, uMsg, wParam, lParam );

	case WM_CREATE:
		break;

    case WM_MDIACTIVATE:
        {
            g_pNikPEViewer = (CNikPEViewer *)GetWindowLong( hWnd, GWL_USERDATA );
            if(g_pNikPEViewer)
            {
                SendMessage( g_hwndStatus, SB_SETTEXT, 0
                    , (LPARAM)g_pNikPEViewer->GetModuleFileName().c_str());
            }
        }
        break;

    case OPEN_RESOURCE_ITEM:
        {
        CNikPEViewer * pNikPEViewer = (CNikPEViewer *)GetWindowLong( hWnd, GWL_USERDATA );
        if(pNikPEViewer)
        {
            try
            {
                RESOURCELIST * pResourceList = (RESOURCELIST *)lParam;
                if( pResourceList )
                {
                    if(0x01 == pResourceList->m_nType)      //  Cursor
				    {
                        HWND hParent = NULL;
                        CNikBitmapEditor * pCursorEditor = NULL;
                        /// create just one Cursor view window
                        if(0 == pNikPEViewer->m_vCursorEditorList.size())
                        {
                            hParent = hWnd;
                            pCursorEditor = new CNikBitmapEditor;
                            pNikPEViewer->m_nChildCount++;
                            pNikPEViewer->m_vCursorEditorList.push_back( pCursorEditor );
                        }
                        else
                            pCursorEditor = pNikPEViewer->m_vCursorEditorList.at(0);

                        if(pCursorEditor)
                        {
                            int nFlag = (pNikPEViewer->m_nChildCount<<16) | (CURSOR_DATA&0xFFFF);
					        if(!pCursorEditor->CreateBitmapWindowBuf( hParent
                                    , &pNikPEViewer->m_pPEBuffer[pResourceList->m_nOffsetToData]
                                    , pResourceList->m_chName, pResourceList->m_nSize, nFlag))
                                return -1;
                        }
				    }
				    else if(0x02 == pResourceList->m_nType)      //  Bitmap
				    {
                        HWND hParent = NULL;
                        CNikBitmapEditor * pBitmapEditor = NULL;
                        /// create just one bitmap view window
                        if(0 == pNikPEViewer->m_vBitmapEditorList.size())
                        {
                            hParent = hWnd;
                            pBitmapEditor = new CNikBitmapEditor;
                            pNikPEViewer->m_nChildCount++;
                            pNikPEViewer->m_vBitmapEditorList.push_back( pBitmapEditor );
                        }
                        else
                            pBitmapEditor = pNikPEViewer->m_vBitmapEditorList.at(0);

                        if(pBitmapEditor)
                        {
                            int nFlag = (pNikPEViewer->m_nChildCount<<16) | (BITMAP_DATA&0xFFFF);
					        if(!pBitmapEditor->CreateBitmapWindowBuf( hParent
                                    , &pNikPEViewer->m_pPEBuffer[pResourceList->m_nOffsetToData]
                                    , pResourceList->m_chName, pResourceList->m_nSize, nFlag))
                                return -2;
                        }
				    }
                    else if(0x03 == pResourceList->m_nType)      //  Icon
				    {
                        HWND hParent = NULL;
                        CNikBitmapEditor * pIconEditor = NULL;
                        /// create just one Icon view window
                        if(0 == pNikPEViewer->m_vIconEditorList.size())
                        {
                            hParent = hWnd;
                            pIconEditor = new CNikBitmapEditor;
                            pNikPEViewer->m_nChildCount++;
                            pNikPEViewer->m_vIconEditorList.push_back( pIconEditor );
                        }
                        else
                            pIconEditor = pNikPEViewer->m_vIconEditorList.at(0);

                        if(pIconEditor)
                        {
                            int nFlag = (pNikPEViewer->m_nChildCount<<16) | (ICON_DATA&0xFFFF);
					        if(!pIconEditor->CreateBitmapWindowBuf( hParent
                                    , &pNikPEViewer->m_pPEBuffer[pResourceList->m_nOffsetToData]
                                    , pResourceList->m_chName, pResourceList->m_nSize, nFlag))
                                return -3;
                        }
				    }
                    else if(0x04 == pResourceList->m_nType)		//	Menu
				    {
                        HWND hParent = NULL;
                        CNikResource * pMenuViewer = NULL;

                        /// create just one dialog view window
                        if(0 == pNikPEViewer->m_vMenuViewerList.size())
                        {
                            hParent = hWnd;
                            pMenuViewer = new CNikResource;
                            pNikPEViewer->m_nChildCount++;
                            pNikPEViewer->m_vMenuViewerList.push_back( pMenuViewer );
                        }
                        else
                            pMenuViewer = pNikPEViewer->m_vMenuViewerList.at(0);

                        if(pMenuViewer)
                        {
                            int nFlag = (pNikPEViewer->m_nChildCount<<16) 
                                            | (pResourceList->m_nType&0xFFFF);
					        if(!pMenuViewer->SetResourceData(hParent, pResourceList
                                            , pNikPEViewer->m_pPEBuffer, nFlag))
                                return -4;
                        }
                    }
                    else if(0x5 == pResourceList->m_nType)		//	Dialog
				    {
                        HWND hParent = NULL;
                        CNikDialog * pDialogViewer = NULL;

                        /// create just one dialog view window
                        if(0 == pNikPEViewer->m_vNikDialogList.size())
                        {
                            hParent = hWnd;
                            pDialogViewer = new CNikDialog;
                            pNikPEViewer->m_nChildCount++;
                            pNikPEViewer->m_vNikDialogList.push_back( pDialogViewer );
                        }
                        else
                            pDialogViewer = pNikPEViewer->m_vNikDialogList.at(0);

                        if(pDialogViewer)
                        {
					        if(!pDialogViewer->SetDialogData(hParent, pResourceList
                                , pNikPEViewer->m_pPEBuffer, ((pNikPEViewer->m_nChildCount)<<16)))
                                return -5;
                        }
				    }
                    else if(0x06 == pResourceList->m_nType)		//	String
				    {
                        HWND hParent = NULL;
                        CNikResource * pStringViewer = NULL;

                        /// create just one dialog view window
                        if(0 == pNikPEViewer->m_vStringViewerList.size())
                        {
                            hParent = hWnd;
                            pStringViewer = new CNikResource;
                            pNikPEViewer->m_nChildCount++;
                            pNikPEViewer->m_vStringViewerList.push_back( pStringViewer );
                        }
                        else
                            pStringViewer = pNikPEViewer->m_vStringViewerList.at(0);

                        if(pStringViewer)
                        {
                            int nFlag = (pNikPEViewer->m_nChildCount<<16) 
                                            | (pResourceList->m_nType&0xFFFF);
					        if(!pStringViewer->SetResourceData(hParent, pResourceList
                                            , pNikPEViewer->m_pPEBuffer, nFlag))
                                return -6;
                        }
                    }
                    else if(0x09 == pResourceList->m_nType)		//	Accelarator keys
				    {
                        HWND hParent = NULL;
                        CNikResource * pAccelaratorViewer = NULL;

                        /// create just one dialog view window
                        if(0 == pNikPEViewer->m_vAccelaratorViewerList.size())
                        {
                            hParent = hWnd;
                            pAccelaratorViewer = new CNikResource;
                            pNikPEViewer->m_nChildCount++;
                            pNikPEViewer->m_vAccelaratorViewerList.push_back( pAccelaratorViewer );
                        }
                        else
                            pAccelaratorViewer = pNikPEViewer->m_vAccelaratorViewerList.at(0);

                        if(pAccelaratorViewer)
                        {
                            int nFlag = (pNikPEViewer->m_nChildCount<<16) 
                                            | (pResourceList->m_nType&0xFFFF);
					        if(!pAccelaratorViewer->SetResourceData(hParent, pResourceList
                                            , pNikPEViewer->m_pPEBuffer, nFlag))
                                return -9;
                        }
                    }
                    else if(0x10 == pResourceList->m_nType)		//	Version
				    {
                        HWND hParent = NULL;
                        CNikResource * pVersionViewer = NULL;

                        /// create just one dialog view window
                        if(0 == pNikPEViewer->m_vVersionViewerList.size())
                        {
                            hParent = hWnd;
                            pVersionViewer = new CNikResource;
                            pNikPEViewer->m_nChildCount++;
                            pNikPEViewer->m_vVersionViewerList.push_back( pVersionViewer );
                        }
                        else
                            pVersionViewer = pNikPEViewer->m_vVersionViewerList.at(0);

                        if(pVersionViewer)
                        {
                            int nFlag = (pNikPEViewer->m_nChildCount<<16) 
                                            | (pResourceList->m_nType&0xFFFF);
					        if(!pVersionViewer->SetResourceData(hParent, pResourceList
                                            , pNikPEViewer->m_pPEBuffer, nFlag))
                                return -0x10;
                        }
                    }
                    else if(0x0B == pResourceList->m_nType)		//	Message
				    {
                        HWND hParent = NULL;
                        CNikResource * pMessageViewer = NULL;

                        /// create just one dialog view window
                        if(0 == pNikPEViewer->m_vMessageViewerList.size())
                        {
                            hParent = hWnd;
                            pMessageViewer = new CNikResource;
                            pNikPEViewer->m_nChildCount++;
                            pNikPEViewer->m_vMessageViewerList.push_back( pMessageViewer );
                        }
                        else
                            pMessageViewer = pNikPEViewer->m_vMessageViewerList.at(0);

                        if(pMessageViewer)
                        {
                            int nFlag = (pNikPEViewer->m_nChildCount<<16) 
                                            | (pResourceList->m_nType&0xFFFF);
					        if(!pMessageViewer->SetResourceData(hParent, pResourceList
                                        , pNikPEViewer->m_pPEBuffer, nFlag))
                                return -0x0B;
                        }
                    }
                    else
                        return -100;
                }
            }
            catch(...)
            {
                return -101;
            }
        }
        }
        break;

    case WM_NOTIFY:
        if(g_pNikPEViewer)
        {
            LPNMHDR  pnmh = (LPNMHDR) lParam;
		    switch(pnmh->idFrom)
			{
			    case IDC_TREEVIEW:
                    return g_pNikPEViewer->m_cPEVTreeCtrl.Tree_Notify( hWnd, lParam );
                case ID_LISTVIEW:
                    return DefMDIChildProc( hWnd, uMsg, wParam, lParam );
			}
        }
      break;

	case WM_DESTROY:
        {
            g_pNikPEViewer->m_cDisAssemblyList.StopDisAssemblyThread();
			int nSize = g_pNikPEViewer->m_vBitmapEditorList.size();
			CNikBitmapEditor * pBitmapEditor;
			for(int i=0;i<nSize;i++)
			{
				pBitmapEditor = g_pNikPEViewer->m_vBitmapEditorList.at(i);
				delete pBitmapEditor;
			}
			g_pNikPEViewer->m_vBitmapEditorList.clear();

            nSize = g_pNikPEViewer->m_vNikDialogList.size();
			CNikDialog * pNikDialog;
			for(int i=0;i<nSize;i++)
			{
				pNikDialog = g_pNikPEViewer->m_vNikDialogList.at(i);
				delete pNikDialog;
			}
            g_pNikPEViewer->m_vNikDialogList.clear();

            nSize = g_pNikPEViewer->m_vResourceViewerList.size();
			CNikResource * pResourceViewer;
			for(int i=0;i<nSize;i++)
			{
				pResourceViewer = g_pNikPEViewer->m_vResourceViewerList.at(i);
				delete pResourceViewer;
			}
            g_pNikPEViewer->m_vResourceViewerList.clear();
		}
        PostMessage(GetParent(hWnd), WM_PE_VIEWER_DISTORY, 0, (LPARAM)hWnd);
		return DefMDIChildProc( hWnd, uMsg, wParam, lParam );

	default:
		return DefMDIChildProc( hWnd, uMsg, wParam, lParam );
    }
    return 0;
}

int CNikPEViewer::ShowCodeDisassembly(int nShow)
{
    if(GetImageNtHeaders().OptionalHeader.Magic != 0x010b)
    {
        ::MessageBox(m_hWnd, _T("Unsupported assembly code!"), _T("Error"), MB_OK);
        return 0;
    }

    if(!m_cDisAssemblyList.GetWindowHandle())
    {
        RECT rc, rc_1;
        GetClientRect(GetWindowHandle(), &rc);

        int nHeight = rc.bottom - rc.top;
        int nWidth = rc.right - rc.left;
        int nWindow = 5;

        rc_1.left = (nWidth/nWindow)*2;
        rc_1.right = rc_1.left + ((nWidth/nWindow) * 3);
        rc_1.top = 22;
        rc_1.bottom = rc_1.top + nHeight/2;
        //m_cDisAssemblyList.CreatePEListViewCtrl(m_hInstance, m_hWnd, rc_1, _T("Code section dis-assembly view"));
        //m_cDisAssemblyList.FillPEVListViewInfo( this );
        m_cDisAssemblyList.NikDissembeInitInstance( m_hWnd, rc_1, _T("Code section dis-assembly view")
            , &m_stExecutableInfo, &g_cNikDisassembly );
        return SetWindowPos( m_cDisAssemblyList.GetWindowHandle(), HWND_TOP
                , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
    }

    if(nShow)
        return SetWindowPos( m_cDisAssemblyList.GetWindowHandle(), HWND_TOP
            , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
    else
    {
        if(TRUE == IsWindowVisible( m_cDisAssemblyList.GetWindowHandle() ))
            return ShowWindow( m_cDisAssemblyList.GetWindowHandle(), SW_HIDE );
        else
            return SetWindowPos( m_cDisAssemblyList.GetWindowHandle(), HWND_TOP
                , 0, 0, 0, 0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOSIZE);
    }
}