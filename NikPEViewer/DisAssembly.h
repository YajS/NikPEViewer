
#pragma once

#define     R_M_OPCODE          1
#define     R_M8_OPCODE         2
#define     D8_OPCODE           3
#define     D32_OPCODE          4
#define     R32_OPCODE          5
#define     R8_OPCODE           6
#define     IMM8_OPCODE         7
#define     IMM32_OPCODE        8
#define     REL8_OPCODE         9
#define     REL32_OPCODE        10
#define     SREG_OPCODE         11
#define     EAX_OPCODE          12
#define     AX_OPCODE           13
#define     AL_OPCODE           14
#define     MOFFS32_OPCODE      15
#define     D16_OPCODE          16
#define     R16_OPCODE          17
#define     IMM16_OPCODE        18
#define     REL16_OPCODE        19
#define     MOFFS8_OPCODE       20

typedef struct tagTHREEPART
{
	BYTE rm     : 3;	
	BYTE reg    : 3;
	BYTE mod    : 2;	
}THREEPART;

typedef struct _INSTRUCTION_OPCODE
{
	short int bOpNumber[5];
	TCHAR strNumber[64];
	TCHAR strOpcode[64];
    int   nSlashPresent;
    BYTE  bOpcodeFlag[8];
    vector<_tstring> strList;
    //vector<int> vOpcodeFlag;

public:
	_INSTRUCTION_OPCODE()
	{
		bOpNumber[0] = -1;
		bOpNumber[1] = -1;
		bOpNumber[2] = -1;
		bOpNumber[3] = -1;
		bOpNumber[4] = -1;

        bOpcodeFlag[0] = 0;
        bOpcodeFlag[1] = 0;
        bOpcodeFlag[2] = 0;
        bOpcodeFlag[3] = 0;
        bOpcodeFlag[4] = 0;
        bOpcodeFlag[5] = 0;
        bOpcodeFlag[6] = 0;
        bOpcodeFlag[7] = 0;

        nSlashPresent= -1;

        strNumber[0] = strNumber[63] = 0;
        strOpcode[0] = strOpcode[63] = 0;
	}
	~_INSTRUCTION_OPCODE()
	{
	}
}INSTRUCTION_OPCODE;

typedef vector<INSTRUCTION_OPCODE *> INSTRUCTION_LIST; 
typedef vector<int> INDEX_INT_LIST; 

class CNikDisassembly
{
    INDEX_INT_LIST      m_vIndexList[256];
    INSTRUCTION_LIST	m_vInstructionList;

public:
    CNikDisassembly();
    virtual ~CNikDisassembly();

    void CleanUp();
    int ConvertCharToNumber(TCHAR ch);
    int DumpTextOpcodeFile(_tstring strFileName);
    int DumpSortedOpcodeFile(_tstring strFileName);
    int LoadSortedOpcodeFile(_tstring strFileName);
    int Fill_OpNumber(INSTRUCTION_OPCODE * pOpcode);
    _tstring GetFormattedStr(BYTE * pBuf,int nFlag);
    _tstring GetNoString(_tstring strBase, int nNo);
    int ExtractString(TCHAR * chBuffer, int nStrLen);
    int GetLineOffset(TCHAR * chBuffer, int nStrLen);
    int ParserIntructionSetFile(_tstring strFileName);
    int ReturnStringNumber(TCHAR * str, int nStringlen);
    _tstring DisOpcodeReturns(BYTE * pBuff,int &nOffset);
    int RemoveWhiteSpaces(TCHAR * chBuffer, int nStrLen);
    int GetPlusValue(INSTRUCTION_OPCODE * pTemp,int nbop);
    int LoadOpcodeResource(HINSTANCE hInstance, int nResourceID);
    int LoadSortedOpcodeBuffer( BYTE * pBuffer, DWORD dwBufferSize);
    INSTRUCTION_OPCODE * SearchNextOpcode(int &nOffset,BYTE * pBuff);
    int GetStringSeprator(TCHAR chSep, TCHAR * chBuffer, int nStrLen);
    _tstring GetStringWithoutDelimiter(const _tstring &str, int &nOffset);
    int FindStringDelimiter(const _tstring &str, TCHAR &ch, int &nStartDelimiter);
    _tstring ProcessOpcodeStr(BYTE *pBuf, INSTRUCTION_OPCODE * pInstOpcode, int &nOpcodeBytes);
};

