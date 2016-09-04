

#include "stdafx.h"
#include "NikDisAssembly.h"

TCHAR modrm_32[32][50] = {
				{_T("ds:[eax]")},
				{_T("ds:[ecx]")},
				{_T("ds:[edx]")},
				{_T("ds:[ebx]")},
				{_T("s-i-b")},
				{_T("ds:d32")},
				{_T("ds:[esi]")},
				{_T("ds:[eax]")},

				{_T("ds:[eax+d8]")},
				{_T("ds:[ecx+d8]")},
				{_T("ds:[edx+d8]")},
				{_T("ds:[ebx+d8]")},
				{_T("s-i-b")},
				{_T("ss:[ebp+d8]")},
				{_T("ds:[esi+d8]")},
				{_T("ds:[edi+d8]")},

				{_T("ds:[eax+d32]")},
				{_T("ds:[ecx+d32]")},
				{_T("ds:[edx+d32]")},
				{_T("ds:[ebx+d32]")},
				{_T("s-i-b")},
				{_T("ss:[ebp+d32]")},
				{_T("ds:[esi+d32]")},
				{_T("ds:[edi+d32]")},

				{_T("eax|ax|al|mm0|xmm0")},
				{_T("ecx|cx|cl|mm1|xmm1")},
				{_T("edx|dx|dl|mm2|xmm2")},
				{_T("ebx|bx|bl|mm3|xmm3")},
				{_T("esp|sp|ah|mm4|xmm4")},
				{_T("ebp|bp|ch|mm5|xmm5")},
				{_T("esi|si|dh|mm6|xmm6")},
				{_T("edi|di|bh|mm7|xmm7")}};

TCHAR modrm_16[32][50] = {
				{_T("ds:[bx+si]")},
				{_T("ds:[bx+di]")},
				{_T("ds:[bp+si]")},
				{_T("ds:[bp+di]")},
				{_T("ds:si")},
				{_T("ds:di")},
				{_T("ds:d16")},
				{_T("ds:[bx]")},

				{_T("ds:[bx+si+d8]")},
				{_T("ds:[bx+di+d8]")},
				{_T("ds:[bp+si+d8]")},
				{_T("ds:[bp+di+d8]")},
				{_T("ds:[si+d8]")},
				{_T("ss:[di+d8]")},
				{_T("ds:[bp+d8]")},
				{_T("ds:[bx+d8]")},

				{_T("ds:[bx+si+d16]")},
				{_T("ds:[bx+di+d16]")},
				{_T("ds:[bp+si+d16]")},
				{_T("ds:[bp+di+d16]")},
				{_T("ds:[si+d16]")},
				{_T("ss:[di+d16]")},
				{_T("ds:[bp+d16]")},
				{_T("ds:[bx+d16]")},

				{_T("eax|ax|al|mm0|xmm0")},
				{_T("ecx|cx|cl|mm1|xmm1")},
				{_T("edx|dx|dl|mm2|xmm2")},
				{_T("ebx|bx|bl|mm3|xmm3")},
				{_T("esp|sp|ah|mm4|xmm4")},
				{_T("ebp|bp|ch|mm5|xmm5")},
				{_T("esi|si|dh|mm6|xmm6")},
				{_T("edi|di|bh|mm7|xmm7")}};

TCHAR Scale_Factor[4][2] = {
				{_T("1")},
				{_T("2")},
				{_T("4")},
				{_T("8")}};

TCHAR Index_reg[8][4] = {
				{_T("eax")},
				{_T("ecx")},
				{_T("edx")},
				{_T("ebx")},
				{_T("")},
				{_T("ebp")},
				{_T("esi")},
				{_T("edi")}};

TCHAR Seg_reg[6][4] = {
				{_T("es")},
				{_T("cs")},
				{_T("ss")},
				{_T("ds")},
				{_T("fs")},
				{_T("gs")}};

TCHAR Reg[8][50] = {
				{_T("eax|ax|al|mm0|xmm0|st(0)")},
				{_T("ecx|cx|cl|mm1|xmm1|st(1)")},
				{_T("edx|dx|dl|mm2|xmm2|st(2)")},
				{_T("ebx|bx|bl|mm3|xmm3|st(3)")},
				{_T("esp|sp|ah|mm4|xmm4|st(4)")},
				{_T("ebp|bp|ch|mm5|xmm5|st(5)")},
				{_T("esi|si|dh|mm6|xmm6|st(6)")},
				{_T("edi|di|bh|mm7|xmm7|st(7)")}};

TCHAR Reg_1[8][4] = {
				{_T("eax")},
				{_T("ecx")},
				{_T("edx")},
				{_T("ebx")},
				{_T("esp")},
				{_T("ebp")},
				{_T("esi")},
				{_T("edi")}};

TCHAR Reg_2[8][4] = {
				{_T("ax")},
				{_T("cx")},
				{_T("dx")},
				{_T("bx")},
				{_T("sp")},
				{_T("bp")},
				{_T("si")},
				{_T("di")}};

TCHAR Reg_3[8][4] = {
				{_T("al")},
				{_T("cl")},
				{_T("dl")},
				{_T("bl")},
				{_T("ah")},
				{_T("ch")},
				{_T("dh")},
				{_T("bh")}};

TCHAR Reg_4[8][4] = {
				{_T("mm0")},
				{_T("mm1")},
				{_T("mm2")},
				{_T("mm3")},
				{_T("mm4")},
				{_T("mm5")},
				{_T("mm6")},
				{_T("mm7")}};

TCHAR Reg_5[8][8] = {
				{_T("xmm0")},
				{_T("xmm1")},
				{_T("xmm2")},
				{_T("xmm3")},
				{_T("xmm4")},
				{_T("xmm5")},
				{_T("xmm6")},
				{_T("xmm7")}};

TCHAR chDelimiter[] = {_T(' '),_T('.'),_T('\\'),_T('/'),_T('?'),_T(','),_T('\t')
						,_T('\n'),_T('\0'),_T('\r'),_T('~'),_T('!'),_T('@')
						,_T('#'),_T('$'),_T('%'),_T('^'),_T('&'),_T('*')
						,_T('('),_T(')'),_T('-'),_T('_'),_T('='),_T('+')
						,_T('<'),_T('>'),_T(':'),_T(';'),_T('\"'),_T('\'')
						,_T('|'),_T('['),_T(']')};

//TCHAR chDelimiter_1[] = {_T(' '),_T('.'),_T('\\'),_T(','),_T('\t')
//						,_T('\n'),_T('\0'),_T('\r'),_T('^'),_T('*'),_T('(')
//						,_T(')'),_T('-'),_T('='),_T('<'),_T('>'),_T(':')
//						,_T(';'),_T('\"'),_T('\''),_T('|'),_T('['),_T(']')};

TCHAR chDelimiter_1[] = {_T(' '),_T('.'),_T('\\'),_T(','),_T('\t')
						,_T('\n'),_T('\0'),_T('\r'),_T('^'),_T('*')
                        ,_T('-'),_T('='),_T('<'),_T('>')/*,_T(':')*/
						,_T(';'),_T('\"'),_T('\''),_T('|')/*,_T('['),_T(']')*/};

TCHAR chWhitSpaces[] = {_T(' '),_T('\t')};

TCHAR chIndex_RM_MODE[2][256][32]; 
TCHAR chIndex_SIB_MODE[256][16];

int CNikDisassembly::Get_RM_Mode_No_Bytes( BYTE * pBuffer )
{
    THREEPART * pth = (THREEPART *)&pBuffer[0];
    int nNo = (pth->mod<<3)|(pth->rm);
	if(nNo >= 24)
	{
        return 1;
	}
	else
	{
        int nNo = 1;
        int nNos = (pth->mod<<3)|(pth->rm);
        if((0x04 == nNos) || (0x0C == nNos)|| (0x14 == nNos))
		{
            nNo++;
			THREEPART * pth1 = (THREEPART *)&pBuffer[1];

            if((5 == pth1->rm) 
			    && (3 != pth->mod))
			{
				if(0 == pth->mod)
				{
					nNo += 4;
				}
				else if(1 == pth->mod)
				{
					nNo += 1;
				}
				else if(2 == pth->mod)
				{
					nNo += 4;
				}
			}
			else
			{
				if(1 == pth->mod)
				{
					nNo += 1;
				}
				else if(2 == pth->mod)
				{
					nNo += 4;
				}
			}
        }
        else if(5 == nNos)
        {
            nNo += 4;
        }
        else if((nNos > 7) && (nNos < 0x10))
		{
			nNo += 1;
		}
		else if((nNos > 0x0F) && (nNos < 0x18))
		{
            nNo += 4;
		}

        return nNo;
    }
}

void CNikDisassembly::Create_RM_Mode()
{
    memset( chIndex_RM_MODE, 0, sizeof(chIndex_RM_MODE) );
    for(int i=0; i<256; i++)
    {
        BYTE bData = (BYTE)i;
        THREEPART * pth = (THREEPART *)&bData;
		int nNo = (pth->mod<<3)|(pth->rm);
		if(nNo >= 24)
		{
            _tcscpy_s(chIndex_RM_MODE[0][i], 32, Reg_1[nNo-24]);
		}
		else
		{
			_tcscpy_s(chIndex_RM_MODE[0][i], 32, modrm_32[nNo]);
		}

        _tcscpy_s(chIndex_RM_MODE[1][i], 32, Reg_1[pth->reg]);
	}
}

void CNikDisassembly::Create_SIB_Mode()
{
    memset( chIndex_SIB_MODE, 0, sizeof(chIndex_RM_MODE) );
    /*for(int i=0; i<256; i++)
    {
        BYTE bData = (BYTE)i;
        THREEPART * pth = (THREEPART *)&bData;

        THREEPART * pth1 = (THREEPART *)&pBuf[nOpcodeBytes];

		_tstring str1,strIndex = Index_reg[pth1->reg];
		if((pth1->mod > 0) 
			&& (false == strIndex.empty()))
		{
			strIndex += _T('*');
			strIndex += Scale_Factor[pth1->mod];
		}

		str1 = _T('[');
		if(false == strIndex.empty())
		{
			str1 += strIndex;
			str1 += _T('+');
		}

		if((5 == pth1->rm) 
			&& (3 != pth->mod))
		{
			if(0 == pth->mod)
			{
				str1 += GetFormattedStr(&pBuf[nOpcodeBytes],4);
				nOpcodeBytes += 4;
				str1 += _T(']');
			}
			else if(1 == pth->mod)
			{
				str1 += GetFormattedStr(&pBuf[nOpcodeBytes],1);
				nOpcodeBytes += 1;
				str1 += _T("+ebp]");
			}
			else if(2 == pth->mod)
			{
				str1 += GetFormattedStr(&pBuf[nOpcodeBytes],4);
				nOpcodeBytes += 4;
				str1 += _T("+ebp]");
			}
		}
		else
		{
            str1 += Reg_1[pth1->rm];
			if(1 == pth->mod)
			{
				str1 += GetFormattedStr(&pBuf[nOpcodeBytes],1);
				nOpcodeBytes += 1;
			}
			else if(2 == pth->mod)
			{
				str1 += _T('+');

				str1 += GetFormattedStr(&pBuf[nOpcodeBytes],4);
				nOpcodeBytes += 4;
			}

			str1 += _T(']');
		}
		str += str1;
	}*/
}


CNikDisassembly::CNikDisassembly()
{
}

CNikDisassembly::~CNikDisassembly()
{
}

void CNikDisassembly::CleanUp()
{
	int nSize = (int)m_vInstructionList.size();
	INSTRUCTION_OPCODE * pInst_Opcode;
	for(int i=0;i<nSize;i++)
	{
		pInst_Opcode = m_vInstructionList.at(i);
		delete pInst_Opcode;
	}
	m_vInstructionList.clear();
}

int CNikDisassembly::RemoveWhiteSpaces(TCHAR * chBuffer, int nStrLen)
{
	//////////// remove white spaces //////////////
	int nPos = 0;
	if((nStrLen > 0) 
		&& (NULL != chBuffer))
	{
		while(nPos < nStrLen)
		{
			if(_T('\t') == chBuffer[nPos])
			{
				nPos++;
			}
			else if(_T(' ') == chBuffer[nPos])
			{
				nPos++;
			}
			else
				break;
		}
	}
	return nPos;
}

int CNikDisassembly::GetLineOffset(TCHAR * chBuffer, int nStrLen)
{
	int i=0;
	if((nStrLen > 0) 
		&& (NULL != chBuffer))
	{
		for(; i<nStrLen; i++)
		{
			if(_T('\n') == chBuffer[i])
			{
                i++;
				if(_T('\r') == chBuffer[i])
					i++;

				break;
			}
			else if(_T('\r') == chBuffer[i])
			{
                i++;
				if(_T('\n') == chBuffer[i])
					i++;

				break;
			}
		}
	}
	return i;
}

int CNikDisassembly::ConvertCharToNumber(TCHAR ch)
{
	if((ch >= _T('0')) && (ch <= _T('9')))
		return (ch - _T('0'));
	else if((ch >= _T('A')) && (ch <= _T('F')))
		return (ch - _T('A') + 0xA);

	return -1;
}

int CNikDisassembly::ReturnStringNumber(TCHAR * str, int nStringlen)
{
	if((NULL == str) 
		|| (nStringlen <= 0))
		return -1;

	int nNo=0,ntemp;

	for(int k=0;k<(int)nStringlen;k++)
	{
		ntemp = ConvertCharToNumber(str[k]);
		if(-1 != ntemp)
		{
			nNo = (nNo<<(4*k)) + ntemp;
		}
		else
		{
			nNo = -1;
			break;
		}
	}
	return nNo;
}

int CNikDisassembly::GetStringSeprator(TCHAR chSep, TCHAR * chBuffer, int nStrLen)
{
	int i=0;
	if((nStrLen > 0) 
		&& (NULL != chBuffer))
	{
		for(; i<nStrLen; i++)
		{
			if(chSep == chBuffer[i])
			{
				break;
			}
		}
	}

	return i;
}

int CNikDisassembly::FindStringDelimiter(const _tstring &str, TCHAR &ch, int &nStartDelimiter)
{
	int i,j;
	////////// remove starting delimiter if any
	for(i=0;i<(int)str.size();i++)
	{
		ch = str.at(i);
		for(j=0;j<(int)(sizeof(chDelimiter_1)/sizeof(TCHAR));j++)
		{
			if(ch == chDelimiter_1[j])
				break;
		}
		if(ch != chDelimiter_1[j])
			break;
	}
	nStartDelimiter = i;
	/////////////////////////////////////////
	for(;i<(int)str.size();i++)
	{
		ch = str.at(i);
		for(j=0;j<(int)(sizeof(chDelimiter_1)/sizeof(TCHAR));j++)
		{
			if(ch == chDelimiter_1[j])
				return i;
		}
	}
	return -1;
}

_tstring CNikDisassembly::GetStringWithoutDelimiter(const _tstring &str, int &nOffset)
{
	_tstring strTemp;
	TCHAR ch;
	int nStart,nOff = FindStringDelimiter(str,ch,nStart);
	if(nOff > 0)
	{
		strTemp = str.substr(nStart,nOff-nStart);
	}
	else
	{
		strTemp = str.substr(nStart,str.length()-nStart);
	}
	nOffset = nOff;
	return strTemp;
}

_tstring CNikDisassembly::GetNoString(_tstring strBase, int nNo)
{
	int nOff;
	_tstring str,strTemp = strBase;

    int k=0;
	for( ;k<nNo; k++)
	{
		str = GetStringWithoutDelimiter(strTemp,nOff);
		if(-1 == nOff)
        {
            k++;
			break;
        }
		else
			strTemp.erase(0,nOff);
	}
    if(k == nNo)
	    return str;
    else
        return _T("");
}

int CNikDisassembly::ExtractString(TCHAR * chBuffer, int nStrLen)
{
	int nPos = 0;
	if((nStrLen > 0) 
		&& (NULL != chBuffer))
	{
		int nTemp = RemoveWhiteSpaces(chBuffer, nStrLen);
		chBuffer += nTemp;

		for(; nPos<nStrLen; nPos++)
		{
			for(int j=0; j<(int)(sizeof(chDelimiter_1)/sizeof(TCHAR)); j++)
			{
				if(chBuffer[nPos] == chDelimiter_1[j])
					return (nPos + nTemp);
			}
		}
	}

	return nPos;
}

_tstring CNikDisassembly::GetFormattedStr(BYTE * pBuf, int nFlag)
{
	TCHAR chBuf[32];
    chBuf[0] = 0;

	chBuf[0] = _T('\0');

	if(4 == nFlag)
	{
		//_stprintf_s(chBuf,32,_T("0x%02X%02X%02X%02X"),pBuf[3],pBuf[2],pBuf[1],pBuf[0]);
        DWORD * pdwTemp = (DWORD *)pBuf;
        _stprintf_s(chBuf,32,_T("0x%08X"),pdwTemp[0]);
	}
	else if(1 == nFlag)
	{
		int nInt = (char)pBuf[0];
		nInt = abs(nInt);
		if(pBuf[0]>>7)
			_stprintf_s(chBuf,32,_T("-0x%02X"),nInt);
		else
			_stprintf_s(chBuf,32,_T("+0x%02X"),nInt);
	}
	if(2 == nFlag)
	{
        WORD * pwTemp = (WORD *)pBuf;
        _stprintf_s(chBuf,32,_T("0x%04X"),pwTemp[0]);
		//_stprintf_s(chBuf,32,_T("0x%02X%02X"),pBuf[1],pBuf[0]);
	}

	return chBuf;
}

_tstring CNikDisassembly::ProcessOpcodeStr(BYTE *pBuf, INSTRUCTION_OPCODE * pInstOpcode, int &nOpcodeBytes, _tstring strPix, int nFlag)
{
	_tstring str,strTemp;
	int nOff;

	nOpcodeBytes = 0;

	nOff = 0;

	{
		THREEPART * pth = (THREEPART *)&pBuf[0];

		TCHAR chBuf[32];
        int i;
		for(i=0;i<(int)pInstOpcode->strList.size();i++)
		{
			if(0 == i)
			{
				str = pInstOpcode->strList.at(0);
				str += _T(' ');
                str += strPix;
			}
			else
			{
				if(2 <= i)
					str += _T(", ");

				strTemp = pInstOpcode->strList.at(i);
                if(( R_M_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                    || ( R_M8_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                  /*|| ( M64FP_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                    || ( M32FP_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                    || ( M80FP_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                    || ( M16_32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                    || ( M32_32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                    || ( M16_16_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                    || ( M2BYTE_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                    || ( M94_108_OPCODE == pInstOpcode->bOpcodeFlag[i] )*/
                    || ( XMM2_M32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                    || ( MM2_M64_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                  /*|| ( XMM2_M128_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                    || ( XMM2_M64_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                    || ( MM2_M32_OPCODE == pInstOpcode->bOpcodeFlag[i] )*/)
				{
                    //_tstring str12 = chIndex_RM_MODE[0][pBuf[0]];
                    //str12 += _T(" , ");
                    //str12 += chIndex_RM_MODE[1][pBuf[0]];

					nOpcodeBytes++;
					int nNo = (pth->mod<<3)|(pth->rm);
					if(nNo >= 24)
					{
						//if((int)strTemp.find(_T("r/m8")) >= 0)
                        if( R_M8_OPCODE == pInstOpcode->bOpcodeFlag[i] )
						{
                            str += Reg_3[nNo-24];
						}
                        else if( XMM2_M32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                        {
                            str += Reg_5[nNo-24];
                        }
                        else if( MM2_M64_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                        {
                            str += Reg_4[nNo-24];
                        }
                        else if( XMM2_M128_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                        {
                            str += Reg_5[nNo-24];
                        }
                        else if( XMM2_M64_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                        {
                            str += Reg_5[nNo-24];
                        }
                        else if( MM2_M32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
                        {
                            str += Reg_4[nNo-24];
                        }
						else
						{
                            str += Reg_1[nNo-24];
						}
					}
					else
					{
						int nPos;
						_tstring strTemp;
                        strTemp = (TCHAR *)modrm_32[nNo];

                        if((0x04 == nNo) || (0x0C == nNo)|| (0x14 == nNo))
						{
							THREEPART * pth1 = (THREEPART *)&pBuf[nOpcodeBytes];
							nOpcodeBytes += 1;

							_tstring str1,strIndex = Index_reg[pth1->reg];
							if((pth1->mod > 0) 
								&& (false == strIndex.empty()))
							{
								strIndex += _T('*');
								strIndex += Scale_Factor[pth1->mod];
							}

							str1 = _T('[');
							if(false == strIndex.empty())
							{
								str1 += strIndex;
								str1 += _T('+');
							}

							if((5 == pth1->rm) 
								&& (3 != pth->mod))
							{
								if(0 == pth->mod)
								{
									str1 += GetFormattedStr(&pBuf[nOpcodeBytes],4);
									nOpcodeBytes += 4;
									str1 += _T(']');
								}
								else if(1 == pth->mod)
								{
									str1 += GetFormattedStr(&pBuf[nOpcodeBytes],1);
									nOpcodeBytes += 1;
									str1 += _T("+ebp]");
								}
								else if(2 == pth->mod)
								{
									str1 += GetFormattedStr(&pBuf[nOpcodeBytes],4);
									nOpcodeBytes += 4;
									str1 += _T("+ebp]");
								}
							}
							else
							{
                                str1 += Reg_1[pth1->rm];
								if(1 == pth->mod)
								{
									str1 += GetFormattedStr(&pBuf[nOpcodeBytes],1);
									nOpcodeBytes += 1;
								}
								else if(2 == pth->mod)
								{
									str1 += _T('+');

									str1 += GetFormattedStr(&pBuf[nOpcodeBytes],4);
									nOpcodeBytes += 4;
								}

								str1 += _T(']');
							}
							str += str1;
						}
						else if((nPos=(int)strTemp.find(_T("d8"))) >= 0)
						{
							if(_T('+') == strTemp.data()[nPos-1])
								strTemp.erase(--nPos,1);

							strTemp.erase(nPos,2);
							strTemp.insert(nPos,GetFormattedStr(&pBuf[nOpcodeBytes],1));
							str += strTemp;
							nOpcodeBytes += 1;
						}
						else if((nPos=(int)strTemp.find(_T("d32"))) >= 0)
						{
							strTemp.erase(nPos,3);
							strTemp.insert(nPos,GetFormattedStr(&pBuf[nOpcodeBytes],4));
							str += strTemp;
							nOpcodeBytes += 4;
						}
						else if((nPos=(int)strTemp.find(_T("d16"))) >= 0)
						{
							strTemp.erase(nPos,3);
							strTemp.insert(nPos,GetFormattedStr(&pBuf[nOpcodeBytes],2));
							str += strTemp;
							nOpcodeBytes += 2;
						}
						else
						{
							str += strTemp;
						}
					}
				}
                else if( R32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
                    str += Reg_1[pth->reg];
				}
                else if( R8_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
                    str += Reg_3[pth->reg];
				}
                else if( R16_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
                    str += Reg_2[pth->reg];
				}
                else if( IMM8_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					_stprintf_s(chBuf,32,_T("0x%02X"),pBuf[nOpcodeBytes]);
					str += chBuf;
					nOpcodeBytes++;
				}
                else if( IMM32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
                    if(TRUE & nFlag)
                    {
                        str += GetFormattedStr(&pBuf[nOpcodeBytes],2);
					    nOpcodeBytes += 2;
                    }
                    else
                    {
					    str += GetFormattedStr(&pBuf[nOpcodeBytes],4);
					    nOpcodeBytes += 4;
                    }
				}
                else if( IMM16_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					str += GetFormattedStr(&pBuf[nOpcodeBytes],2);
					nOpcodeBytes += 2;
				}
                else if( REL8_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					_stprintf_s(chBuf,32,_T("0x%02X"),pBuf[nOpcodeBytes]);
					str += chBuf;
					nOpcodeBytes++;
				}
                else if( MOFFS8_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					_stprintf_s(chBuf,32,_T("0x%02X"),pBuf[nOpcodeBytes]);
					str += chBuf;
					nOpcodeBytes++;
				}
                else if( MOFFS32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
                    if(TRUE & nFlag)
                    {
                        str += GetFormattedStr(&pBuf[nOpcodeBytes],2);
					    nOpcodeBytes += 2;
                    }
                    else
                    {
					    str += GetFormattedStr(&pBuf[nOpcodeBytes],4);
					    nOpcodeBytes += 4;
                    }
				}
                else if( REL32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
                    if(TRUE & nFlag)
                    {
                        str += GetFormattedStr(&pBuf[nOpcodeBytes],2);
					    nOpcodeBytes += 2;
                    }
                    else
                    {
					    str += GetFormattedStr(&pBuf[nOpcodeBytes],4);
					    nOpcodeBytes += 4;
                    }
				}
                else if( REL16_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					str += GetFormattedStr(&pBuf[nOpcodeBytes],2);
					nOpcodeBytes += 2;
				}
                else if( SREG_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					str += Seg_reg[pth->reg];
				}
                else if( M32INT_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
                    str += Reg_1[pth->reg];
                    nOpcodeBytes++;
				}
                else if( M16INT_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
                    str += Reg_2[pth->reg];
                    nOpcodeBytes++;
				}
                else if( XMM1_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
                    str += Reg_5[pth->reg];
				}
                else if( MM1_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
                    str += Reg_4[pth->reg];
				}
				else
				{
					str += strTemp;
				}
			}
		}
	}

	return str;
}

int CNikDisassembly::GetPlusValue(INSTRUCTION_OPCODE * pTemp, int nbop)
{
	int nPlus = 0;
    if(NULL != pTemp->strNumber[0])
	{
		_tstring strTemp = pTemp->strNumber;
		int nOpt = (int)strTemp.find(_T('+'));
		if(nOpt >= 0)
		{
			int nOp = nbop - (BYTE)pTemp->bOpNumber[0];
			if(nOp < 8)
			{
				nPlus = (nOp<<8) | 1;
			}
			else
				return -1;	//	cannot find the opcode ???
		}
	}
	return nPlus;
}

INSTRUCTION_OPCODE * CNikDisassembly::SearchNextOpcode(int &nOffset, BYTE * pBuff)
{
	BYTE bOp = pBuff[nOffset];

    if(1 == m_nOpcodeList[bOp])
    {
        INSTRUCTION_OPCODE * pTemp = m_pIntOpcodeList[bOp];
        if(-1 == pTemp->bOpNumber[1])
            nOffset += 1;
        else if(-1 == pTemp->bOpNumber[2])
            nOffset += 2;
        else if(-1 == pTemp->bOpNumber[3])
            nOffset += 3;
        else if(-1 == pTemp->bOpNumber[4])
            nOffset += 4;
        return pTemp;
    }

    if(!m_nOpcodeList[bOp])
    {
        return NULL;
    }

    if(0x0F == bOp)
    {
        BYTE bOp1 = pBuff[nOffset+1];
        if(1 == m_nOpcodeList_OF[bOp1])
        {
            INSTRUCTION_OPCODE * pTemp = m_pOpcodeList_OF[bOp1];
            if(-1 == pTemp->bOpNumber[2])
                nOffset += 2;
            else if(-1 == pTemp->bOpNumber[3])
                nOffset += 3;
            else if(-1 == pTemp->bOpNumber[4])
                nOffset += 4;

            return pTemp;
        }
    }

    if(0xF2 == bOp)
    {
        if(0x0F != pBuff[nOffset+1])
            return NULL;
    }

    if(0xF3 == bOp)
    {
        if(0x0F != pBuff[nOffset+1])
        {
            if(0x6C > pBuff[nOffset+1])
                return NULL;
        }
    }

	INSTRUCTION_OPCODE * pTemp = NULL, * pTemp2 = NULL;

	//if(bOpp1 >= bOp)
	{
        pTemp2 = NULL;
        int ii = nOffset;
        int j = m_vIndexList[bOp].at(0);
        BYTE bOp1 = pBuff[nOffset+1];

        /////-----------------------------------------------
        if(0x0F == bOp)
        {
            j = m_nOpcodeList_OF[bOp1];

            if((0x00 == bOp1)
                || (0x01 == bOp1)
                || (0x18 == bOp1))
            {
                THREEPART * pth = (THREEPART *)&pBuff[nOffset+2];
                pTemp = m_vInstructionList.at(j + pth->reg);
                if(pth->reg == pTemp->nSlashPresent)
                {
                    nOffset += 2;
                    return pTemp;
                }
                else
                    return NULL;
            }
            else if(0xAE == bOp1)
            {
                THREEPART * pth = (THREEPART *)&pBuff[nOffset+2];
                int nIndex = pth->reg;
                if(nIndex > 4)
                    nIndex--;
                pTemp = m_vInstructionList.at(j + nIndex);
                if(pth->reg == pTemp->nSlashPresent)
                {
                    nOffset += 2;
                    return pTemp;
                }
                else
                    return NULL;
            }
            else if( (0x71 == bOp1)
                    || (0x72 == bOp1))
            {
                THREEPART * pth = (THREEPART *)&pBuff[nOffset+2];
                int nIndex = pth->reg;
                if(2 == nIndex)
                {
                    nOffset += 2;
                    return m_vInstructionList.at(j);
                }
                else if(4 == nIndex)
                {
                    nOffset += 2;
                    return m_vInstructionList.at(j + 1);
                }
                else if(6 == nIndex)
                {
                    nOffset += 2;
                    return m_vInstructionList.at(j + 2);
                }
                else
                    return NULL;
            }
            else if(0x73 == bOp1)
            {
                THREEPART * pth = (THREEPART *)&pBuff[nOffset+2];
                int nIndex = pth->reg;
                if(2 == nIndex)
                {
                    nOffset += 2;
                    return m_vInstructionList.at(j);
                }
                else if(3 == nIndex)
                {
                    nOffset += 2;
                    return m_vInstructionList.at(j + 1);
                }
                else if(6 == nIndex)
                {
                    nOffset += 2;
                    return m_vInstructionList.at(j + 2);
                }
                else if(7 == nIndex)
                {
                    nOffset += 2;
                    return m_vInstructionList.at(j + 3);
                }
                else
                    return NULL;
            }
            else if(0xBA == bOp1)
            {
                THREEPART * pth = (THREEPART *)&pBuff[nOffset+2];
                pTemp = m_vInstructionList.at(j + pth->reg - 4);
                if(pth->reg == pTemp->nSlashPresent)
                {
                    nOffset += 2;
                    return pTemp;
                }
                else
                    return NULL;
            }
        }
        else if(bOp == 0xFF)
        {
            THREEPART * pth = (THREEPART *)&bOp1;
            if((j + pth->reg) < (int)m_vInstructionList.size())
            {
                pTemp = m_vInstructionList.at(j + pth->reg);
                if(pth->reg == pTemp->nSlashPresent)
                {
                    nOffset += 1;
                    return pTemp;
                }
                else
                    return NULL;
            }
            else
                return NULL;
        }
        else if(bOp == 0xFE)
        {
            THREEPART * pth = (THREEPART *)&bOp1;
            if((j + pth->reg) < (int)m_vInstructionList.size())
            {
                pTemp = m_vInstructionList.at(j + pth->reg);
                if(pth->reg == pTemp->nSlashPresent)
                {
                    nOffset += 1;
                    return pTemp;
                }
                else
                    return NULL;
            }
            else
                return NULL;
        }
        else if((bOp == 0x80)
                || (bOp == 0x81)
                || (bOp == 0x83))
        {
            THREEPART * pth = (THREEPART *)&bOp1;
            pTemp = m_vInstructionList.at(j + pth->reg);
            if(pth->reg == pTemp->nSlashPresent)
            {
                nOffset += 1;
                return pTemp;
            }
            else
                return NULL;
        }
        else if((bOp == 0xD0)
                || (bOp == 0xD1)
                || (bOp == 0xD2)
                || (bOp == 0xD3)
                || (bOp == 0xC0)
                || (bOp == 0xC1))
        {
            THREEPART * pth = (THREEPART *)&bOp1;
            int nIndex = pth->reg;
            if(nIndex > 6)
                nIndex--;
            pTemp = m_vInstructionList.at(j + nIndex);
            if(pth->reg == pTemp->nSlashPresent)
            {
                nOffset += 1;
                return pTemp;
            }
            else
                return NULL;
        }
        else if((bOp == 0xF6)
                || (bOp == 0xF7))
        {
            THREEPART * pth = (THREEPART *)&bOp1;
            int nIndex = pth->reg;
            if( nIndex )
                nIndex--;
            pTemp = m_vInstructionList.at(j + nIndex);
            if(pth->reg == pTemp->nSlashPresent)
            {
                nOffset += 1;
                return pTemp;
            }
            else
                return NULL;
        }
        else if(bOp == 0xD8) 
        {
            int i = bOp1 - 0xC0;
            if(i >= 0)
            {
                j += 8;
                pTemp = m_vInstructionList.at(j + i);
                if(bOp1 == pTemp->bOpNumber[1])
                {
                    nOffset += 2;
                    return pTemp;
                }
            }

            THREEPART * pth = (THREEPART *)&bOp1;
            pTemp = m_vInstructionList.at(j + pth->reg);
            if(pth->reg == pTemp->nSlashPresent)
            {
                nOffset += 1;
                return pTemp;
            }
            else
                return NULL;
        }
        else if(bOp == 0xDA) 
        {
            int i = bOp1 - 0xC0;
            if(i >= 0)
            {
                j += 8;
                if(i <= 0x1F)
                {
                    pTemp = m_vInstructionList.at(j + i);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
                else if(i == 0x29)
                {
                    pTemp = m_vInstructionList.at(j + 0x20);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
            }
            else
            {
                THREEPART * pth = (THREEPART *)&bOp1;
                pTemp = m_vInstructionList.at(j + pth->reg);
                if(pth->reg == pTemp->nSlashPresent)
                {
                    nOffset += 1;
                    return pTemp;
                }
            }

            return NULL;
        }
        else if(bOp == 0xDC) 
        {
            int i = bOp1 - 0xC0;
            if(i >= 0)
            {
                j += 8;
                if(i <= 0x0F)
                {
                    pTemp = m_vInstructionList.at(j + i);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
                else if(i >= 0x20)
                {
                    i = bOp1 - 0xE0 + 0x10;
                    pTemp = m_vInstructionList.at(j + i);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
            }
            else
            {
                THREEPART * pth = (THREEPART *)&bOp1;
                pTemp = m_vInstructionList.at(j + pth->reg);
                if(pth->reg == pTemp->nSlashPresent)
                {
                    nOffset += 1;
                    return pTemp;
                }
            }

            return NULL;
        }
        else if(bOp == 0xD9) 
        {
            int i = bOp1 - 0xC0;
            if(i >= 0)
            {
                j += 7;
                if(i <= 0x10)
                {
                    pTemp = m_vInstructionList.at(j + i);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
                else if(bOp1 == 0xE0)
                {
                    pTemp = m_vInstructionList.at(j + 0x11);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
                else if(bOp1 == 0xE1)
                {
                    pTemp = m_vInstructionList.at(j + 0x12);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
                else if((bOp1 >= 0xE4)
                    && (bOp1 <= 0xEE))
                {
                    i = bOp1 - 0xE4 + 0x11;
                    pTemp = m_vInstructionList.at(j + i);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
                else if(bOp1 >= 0xF0)
                {
                    i = bOp1 - 0xF0 + 28;
                    pTemp = m_vInstructionList.at(j + i);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
            }
            else
            {
                THREEPART * pth = (THREEPART *)&bOp1;
                int nIndex = pth->reg;
                if(nIndex)
                    nIndex--;
                pTemp = m_vInstructionList.at(j + nIndex);
                if(pth->reg == pTemp->nSlashPresent)
                {
                    nOffset += 1;
                    return pTemp;
                }
            }

            return NULL;
        }
        else if(bOp == 0xDB) 
        {
            int i = bOp1 - 0xC0;
            if(i >= 0)
            {
                j += 6;
                if(i <= 0x1F)
                {
                    pTemp = m_vInstructionList.at(j + i);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
                else if(bOp1 == 0xE2)
                {
                    pTemp = m_vInstructionList.at(j + 0x20);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
                else if(bOp1 == 0xE3)
                {
                    pTemp = m_vInstructionList.at(j + 0x21);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
                else if((bOp1 >= 0xE8)
                    && (bOp1 <= 0xF7))
                {
                    i = bOp1 - 0xE8 + 34;
                    pTemp = m_vInstructionList.at(j + i);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
            }
            else
            {
                THREEPART * pth = (THREEPART *)&bOp1;
                int nIndex = pth->reg;
                if(nIndex > 3)
                    nIndex--;
                if(nIndex > 6)
                    nIndex--;
                pTemp = m_vInstructionList.at(j + nIndex);
                if(pth->reg == pTemp->nSlashPresent)
                {
                    nOffset += 1;
                    return pTemp;
                }
            }

            return NULL;
        }
        else if(bOp == 0xDD)
        {
            int i = bOp1 - 0xC0;
            if(i >= 0)
            {
                j += 7;
                if(i <= 0x07)
                {
                    pTemp = m_vInstructionList.at(j + i);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
                else if((bOp1 >= 0xD0)
                    && (bOp1 <= 0xEF))
                {
                    i = bOp1 - 0xD0 + 8;
                    pTemp = m_vInstructionList.at(j + i);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
            }
            else
            {
                THREEPART * pth = (THREEPART *)&bOp1;
                int nIndex = pth->reg;
                if(nIndex > 5)
                    nIndex--;
                pTemp = m_vInstructionList.at(j + nIndex);
                if(pth->reg == pTemp->nSlashPresent)
                {
                    nOffset += 1;
                    return pTemp;
                }
            }

            return NULL;
        }
        else if(bOp == 0xDE) 
        {
            int i = bOp1 - 0xC0;
            if(i >= 0)
            {
                j += 8;
                if(i <= 0x10)
                {
                    pTemp = m_vInstructionList.at(j + i);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
                else if(bOp1 == 0xD9)
                {
                    pTemp = m_vInstructionList.at(j + 0x10);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
                else if(bOp1 >= 0xE0)
                {
                    i = bOp1 - 0xE0 + 0x11;
                    pTemp = m_vInstructionList.at(j + i);
                    if(bOp1 == pTemp->bOpNumber[1])
                    {
                        nOffset += 2;
                        return pTemp;
                    }
                }
            }
            else
            {
                THREEPART * pth = (THREEPART *)&bOp1;
                pTemp = m_vInstructionList.at(j + pth->reg);
                if(pth->reg == pTemp->nSlashPresent)
                {
                    nOffset += 1;
                    return pTemp;
                }
            }

            return NULL;
        }
        else if(bOp == 0xDF) 
        {
            int i = bOp1 - 0xE8;
            if(bOp1 == 0xE0)
            {
                pTemp = m_vInstructionList.at(j + 8);
                if(bOp1 == pTemp->bOpNumber[1])
                {
                    nOffset += 2;
                    return pTemp;
                }
            }
            else if(i >= 0)
            {
                i += 9;
                pTemp = m_vInstructionList.at(j + i);
                if(bOp1 == pTemp->bOpNumber[1])
                {
                    nOffset += 2;
                    return pTemp;
                }
            }
            else
            {
                THREEPART * pth = (THREEPART *)&bOp1;
                pTemp = m_vInstructionList.at(j + pth->reg);
                if(pth->reg == pTemp->nSlashPresent)
                {
                    nOffset += 1;
                    return pTemp;
                }
            }

            return NULL;
        }
        /////-----------------------------------------------

        for(; j<(int)m_vInstructionList.size(); j++)
        {
            pTemp = m_vInstructionList.at(j);

            if(pBuff[nOffset] < pTemp->bOpNumber[0])
                return NULL;

            if(pBuff[nOffset] == pTemp->bOpNumber[0])
            {
                if(-1 == pTemp->bOpNumber[1])
                {
                    nOffset += 1;
                    pTemp2 = pTemp;
                }
                else if(pBuff[nOffset+1] == pTemp->bOpNumber[1])
                {
                    if(-1 == pTemp->bOpNumber[2])
                    {
                        nOffset += 2;
                        pTemp2 = pTemp;
                    }
                    else if(pBuff[nOffset+2] == pTemp->bOpNumber[2])
                    {
                        if(-1 == pTemp->bOpNumber[3])
                        {
                            nOffset += 3;
                            pTemp2 = pTemp;
                        }
                        else if((pBuff[nOffset+3] == pTemp->bOpNumber[3])
                                && (-1 == pTemp->bOpNumber[4]))
                        {
                            nOffset += 4;
                            pTemp2 = pTemp;
                        }
                    }
                }
            }

            if( pTemp2 )
            {
                if(pTemp2->nSlashPresent >= 0)
                {
                    THREEPART * pth = (THREEPART *)&pBuff[nOffset];

                    if(pTemp->nSlashPresent == pth->reg)
                    {
                        return pTemp;
                    }
                    else
                    {
                        nOffset = ii;
                        pTemp2 = NULL;
                    }
                }
                else
                    return pTemp2;
            }
        }
	}

	return NULL;
}

_tstring CNikDisassembly::DisOpcodeReturns(BYTE * pBuff, int &nOffset)
{
	_tstring str, str_1;
	BYTE bOp = pBuff[0];
	nOffset = 0;
	int i=0, nIsO_S = FALSE;

	/////////// special case /////////////
    if(0x10 == bOp)
        return _T("");

	if(0x90 == bOp)
	{
		nOffset++;
		return _T("NOP");
	}

    if( 0x0f == bOp ) 
    {
        if( 0 == m_nOpcodeList_OF[pBuff[1]] )
            return _T("");
    }
	//////////////////////////////////////

    if(0x26 == pBuff[i])
    {
        str_1 = _T("ES: ");
        i++;
    }
    else if(0x2E == pBuff[i])
    {
        str_1 = _T("CS: ");
        i++;
    }
    else if(0x36 == pBuff[i])
    {
        str_1 = _T("SS: ");
        i++;
    }
    else if(0x3E == pBuff[i])
    {
        str_1 = _T("DS: ");
        i++;
    }
    else if(0x64 == pBuff[i])
    {
        str_1 = _T("FS: ");
        i++;
    }
    else if(0x65 == pBuff[i])
    {
        str_1 = _T("GS: ");
        i++;
    }
    else if(0x66 == pBuff[i])
    {
        str_1 = _T("OS: ");
        i++;
        nIsO_S = TRUE;
    }
    else if(0x67 == pBuff[i])
    {
        str_1 = _T("AS: ");
        i++;
    }
    else if(0xF0 == pBuff[i])
    {
        str_1 = _T("LOCK: ");
        i++;
    }
    else if(0xF2 == pBuff[i])
    {
        str_1 = _T("REPNE: ");
        i++;
    }
    else if(0xF3 == pBuff[i])
    {
        str_1 = _T("REPE: ");
        i++;
    }

    //if((0x0F == pBuff[i]) && (0x7E == pBuff[i+1]))
    //    pBuff[i] = pBuff[i];

	INSTRUCTION_OPCODE * pTemp = SearchNextOpcode(i , pBuff);

    int nPlus;

    nPlus = 0;

    if(NULL != pTemp)
    {
        /*//----------------------------------
        int nNoBytes = 0x7FFFFFFF & pTemp->nNoOpcodeBytes;
        if(pTemp->nNoOpcodeBytes & 0x80000000)
        {
            nNoBytes += Get_RM_Mode_No_Bytes( &pBuff[i] );
        }
        if( nIsO_S )
        {
            for(int i=0; i<8; i++)
            {
                if( IMM32_OPCODE == pTemp->bOpcodeFlag[i] )
			    {
				    nNoBytes -= 2;
			    }
                else if( MOFFS32_OPCODE == pTemp->bOpcodeFlag[i] )
			    {
				    nNoBytes -= 2;
			    }
                else if( REL32_OPCODE == pTemp->bOpcodeFlag[i] )
			    {
				    nNoBytes -= 2;
			    }
            }
        }
        if(!str_1.empty())
            nNoBytes++;
        *///----------------------------------

        if(NULL != pTemp->strNumber[0])
	    {
            str = ProcessOpcodeStr(&pBuff[i], pTemp, nPlus, str_1, nIsO_S);

		    nOffset = nPlus + i;

            //nOffset = nNoBytes;
            //str = _T("NOP");
	    }
	    else
		    str = pTemp->strOpcode;
    }

	return str;
}

int CNikDisassembly::Fill_OpNumber(INSTRUCTION_OPCODE * pOpcode)
{
	if(pOpcode
        && (pOpcode->strNumber[0]))
	{
		int nIndex = 0,nTemp;
        int nTemplen = (int)_tcslen(pOpcode->strNumber);
        TCHAR * pBuff = (TCHAR *)pOpcode->strNumber;
        if(pOpcode->strNumber[0])
		{
			TCHAR chNumb[256];
			do
			{
				nTemp = ExtractString(pBuff, nTemplen);
				if(nTemp > 0)
				{
					memcpy(chNumb,pBuff,nTemp*sizeof(TCHAR));
					chNumb[nTemp] = 0;

					int nNo=ReturnStringNumber(chNumb, nTemp);
					if(-1 != nNo)
						pOpcode->bOpNumber[nIndex++] = (short int)nNo;

					pBuff += nTemp;
					nTemplen -= nTemp;

					if(nTemplen > 0)
					{
						nTemp = RemoveWhiteSpaces(pBuff, nTemp);
						pBuff += nTemp;
						nTemplen -= nTemp;
					}
				}
				else
					break;
			}while(nTemplen > 0);
		}
	}

	return 0;
}

int CNikDisassembly::LoadSortedOpcodeBuffer( BYTE * pBuffer, DWORD dwBufferSize)
{
    memset( m_nOpcodeList, 0, sizeof(m_nOpcodeList));
    memset( m_pIntOpcodeList, 0, sizeof(m_pIntOpcodeList));
    memset( m_pOpcodeList_OF, 0, sizeof(m_pOpcodeList_OF));
    memset( m_nOpcodeList_OF, 0, sizeof(m_nOpcodeList_OF));

    Create_RM_Mode();

    if(pBuffer)
	{
        INSTRUCTION_OPCODE stInstOpcode;
        BYTE * pInstOpcode = (BYTE *)pBuffer;
        int nSizeStruct = sizeof(stInstOpcode.bOpNumber) + sizeof(stInstOpcode.strNumber)
                            + sizeof(stInstOpcode.strOpcode);

        DWORD dwRead = dwBufferSize / nSizeStruct;
        for(int i=0;i<(int)dwRead;i++)
        {
            INSTRUCTION_OPCODE * pInstruct = new INSTRUCTION_OPCODE;
            if(pInstruct)
            {
                memcpy(pInstruct, pInstOpcode, nSizeStruct);
                {
                    TCHAR * chTemp = _tcschr(pInstruct->strOpcode, _T('\r'));
                    if(chTemp)
                        chTemp[0] = 0;
                    chTemp = _tcschr(pInstruct->strOpcode, _T('\n'));
                    if(chTemp)
                        chTemp[0] = 0;
                }

                {
                    pInstruct->nNoOpcodeBytes = 0;
                    for(int j=0; j<5; j++)
                    {
                        if(-1 != pInstruct->bOpNumber[j])
                            pInstruct->nNoOpcodeBytes++;
                        else
                            break;
                    }
                }

                {
                    INSTRUCTION_OPCODE * &pstInstruct = pInstruct;//m_vInstructionList.at(i);
                    int nOff = 0,nOffImm = 0, ii = 0;
                    _tstring strTemp, strOp = pstInstruct->strOpcode;

                    do
                    {
                        strTemp = GetStringWithoutDelimiter(&strOp.data()[nOff],nOffImm);
                        if(false == strTemp.empty())
                        {
                            if((int)strTemp.find(_T("r/m8")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R_M8_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("r/m")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R_M_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("d8")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = D8_OPCODE;
                                pstInstruct->nNoOpcodeBytes++;
                            }
                            else if((int)strTemp.find(_T("d32")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = D32_OPCODE;
                                pstInstruct->nNoOpcodeBytes += 4;
                            }
                            else if((int)strTemp.find(_T("r32")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R32_OPCODE;
                            }
                            else if((int)strTemp.find(_T("r8")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R8_OPCODE;
                            }
                            else if((int)strTemp.find(_T("imm8")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = IMM8_OPCODE;
                                pstInstruct->nNoOpcodeBytes++;
                            }
                            else if((int)strTemp.find(_T("imm32")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = IMM32_OPCODE;
                                pstInstruct->nNoOpcodeBytes += 4;
                            }
                            else if((int)strTemp.find(_T("rel8")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = REL8_OPCODE;
                                pstInstruct->nNoOpcodeBytes++;
                            }
                            else if((int)strTemp.find(_T("rel32")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = REL32_OPCODE;
                                pstInstruct->nNoOpcodeBytes += 4;
                            }
                            else if((int)strTemp.find(_T("Sreg")) >= 0)
				            {
                                pstInstruct->bOpcodeFlag[ii] = SREG_OPCODE;
				            }
				            else if((int)strTemp.find(_T("EAX")) >= 0)
				            {
                                pstInstruct->bOpcodeFlag[ii] = EAX_OPCODE;
				            }
				            else if((int)strTemp.find(_T("AX")) >= 0)
				            {
                                pstInstruct->bOpcodeFlag[ii] = AX_OPCODE;
				            }
				            else if((int)strTemp.find(_T("AL")) >= 0)
				            {
                                pstInstruct->bOpcodeFlag[ii] = AL_OPCODE;
				            }
                            else if(((int)strTemp.find(_T("moffs32")) >= 0)
					            || ((int)strTemp.find(_T("moffs16")) >= 0))
                            {
                                pstInstruct->bOpcodeFlag[ii] = MOFFS32_OPCODE;
                                pstInstruct->nNoOpcodeBytes += 4;
                            }
                            else if((int)strTemp.find(_T("d16")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = D16_OPCODE;
                                pstInstruct->nNoOpcodeBytes += 2;
                            }
                            else if((int)strTemp.find(_T("r16")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R16_OPCODE;
                            }
                            else if((int)strTemp.find(_T("imm16")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = IMM16_OPCODE;
                                pstInstruct->nNoOpcodeBytes += 2;
                            }
                            else if((int)strTemp.find(_T("rel16")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = REL16_OPCODE;
                                pstInstruct->nNoOpcodeBytes += 2;
                            }
                            else if((int)strTemp.find(_T("moffs8")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = MOFFS8_OPCODE;
                                pstInstruct->nNoOpcodeBytes++;
                            }
                            else if((int)strTemp.find(_T("m32fp")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R_M_OPCODE; //M32FP_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("m64fp")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R_M_OPCODE; //M64FP_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("m80")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R_M_OPCODE; //M80FP_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("m16int")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = M16INT_OPCODE;
                                pstInstruct->nNoOpcodeBytes++;
                            }
                            else if((int)strTemp.find(_T("m32int")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = M32INT_OPCODE;
                                pstInstruct->nNoOpcodeBytes++;
                            }
                            else if((int)strTemp.find(_T("m64int")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = M64INT_OPCODE;
                            }
                            else if((int)strTemp.find(_T("m16_32")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R_M_OPCODE; //M16_32_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("m32&32")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R_M_OPCODE; //M32_32_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("m16&16")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R_M_OPCODE; //M16_16_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("m2byte")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R_M_OPCODE; //M2BYTE_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("m94_108byte")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R_M_OPCODE; //M94_108_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("xmm2/m32")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = XMM2_M32_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("xmm2/m128")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = XMM2_M32_OPCODE;  // XMM2_M128_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("xmm2/m64")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = XMM2_M32_OPCODE;  // XMM2_M64_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("mm2/m64")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = MM2_M64_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("mm2/m32")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = MM2_M64_OPCODE;  //MM2_M32_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.find(_T("mm/m64")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = MM1_OPCODE;
                            }
                            else if((int)strTemp.find(_T("mm/m32")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = MM1_OPCODE;
                            }
                            else if((int)strTemp.find(_T("xmm1")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = XMM1_OPCODE;
                            }
                            else if((int)strTemp.find(_T("xmm2")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = XMM1_OPCODE;
                            }
                            else if((int)strTemp.find(_T("mm1")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = MM1_OPCODE;
                            }
                            else if((int)strTemp.compare(_T("xmm")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = XMM1_OPCODE;
                                //pstInstruct->bOpcodeFlag[ii] = XMM2_M32_OPCODE;  // XMM_OPCODE;
                                //pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else if((int)strTemp.compare(_T("mm")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = MM2_M64_OPCODE;  // MM_OPCODE;
                                pInstruct->nNoOpcodeBytes |= 0x80000000;
                            }
                            else
                            {
                                pstInstruct->bOpcodeFlag[ii] = 0;
                            }
                            pstInstruct->strList.push_back(strTemp);
                            ii++;
                        }
                        nOff += nOffImm;
                    }
                    while(nOffImm > 0);
                }

                {
                    int nOf = 0;
                    if((nOf = (int)(_tcschr(pInstruct->strNumber, _T('/')) - pInstruct->strNumber)) >= 0)
			        {
                        int nOf1 = ConvertCharToNumber(pInstruct->strNumber[nOf+1]);
                        if((nOf1 >= 0) 
					        && (nOf1 < 8))
				        {
                            pInstruct->nSlashPresent = nOf1;
                        }
                        else if(_T('r') == pInstruct->strNumber[nOf+1])
                        {
                            pInstruct->nNoOpcodeBytes |= 0x80000000;

                            /*for(int j=0; j<256; j++)
                            {
                                INSTRUCTION_OPCODE * pInstruct_1 = new INSTRUCTION_OPCODE;
                                *pInstruct_1 = *pInstruct;
                                _tcscpy_s( pInstruct_1->strOpcode, 63, chIndex_RM_MODE[0][j]);
                                delete pInstruct_1;
                            }*/
                        }
                    }
                }

                m_vInstructionList.push_back( pInstruct );

                BYTE bNos1 = (BYTE)((INSTRUCTION_OPCODE *)pInstOpcode)->bOpNumber[0];
                m_vIndexList[bNos1].push_back(i);

                m_pIntOpcodeList[bNos1] = pInstruct;
                if(m_nOpcodeList[bNos1] > 0)
                    m_pIntOpcodeList[bNos1] = NULL;

                m_nOpcodeList[bNos1]++;

                pInstOpcode += nSizeStruct;

                if(0x0F == pInstruct->bOpNumber[0])
                {
                    if(-1 != pInstruct->bOpNumber[1])
                    {
                        if(!m_nOpcodeList_OF[pInstruct->bOpNumber[1]])
                        {
                            m_pOpcodeList_OF[pInstruct->bOpNumber[1]] = pInstruct;
                            m_nOpcodeList_OF[pInstruct->bOpNumber[1]]++;
                        }
                        else if(1 == m_nOpcodeList_OF[pInstruct->bOpNumber[1]])
                            m_nOpcodeList_OF[pInstruct->bOpNumber[1]] = i - 1;
                    }
                }
            }
            else
                break;
        }
    }

    return 1;
}

int CNikDisassembly::LoadOpcodeResource(HINSTANCE hInstance, int nResourceID)
{
    HRSRC hRes = FindResource(hInstance
                            , MAKEINTRESOURCE(nResourceID)
                            , _T("OPCODE_FILE"));
    if (hRes == NULL)
        return 0;

    DWORD dwSize = SizeofResource( hInstance, hRes );

    HGLOBAL hResLoad = LoadResource(hInstance, hRes); 
    if (hResLoad == NULL)
        return 0;

    BYTE * lpResLock = (BYTE *)LockResource( hResLoad );
    if (lpResLock == NULL)
        return 0;

    LoadSortedOpcodeBuffer( lpResLock, dwSize );

    UnlockResource( hResLoad );

    return 1;
}

int CNikDisassembly::LoadSortedOpcodeFile(_tstring strFileName)
{
    if(strFileName.empty())
        return 0;

    HANDLE hFile = CreateFile(strFileName.c_str(),
                               GENERIC_READ,
                               FILE_SHARE_READ|FILE_SHARE_WRITE,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
     
    if (hFile == INVALID_HANDLE_VALUE) 
        return 0;


    CleanUp();

    DWORD dwFileSize = GetFileSize(hFile,NULL);
	if(dwFileSize)
	{
		BYTE * pBuffer = new BYTE [dwFileSize + 2];
		if(pBuffer)
		{
            pBuffer[dwFileSize] = 0;
			pBuffer[dwFileSize+1] = 0;
			DWORD dwRead = 0;
            ReadFile(hFile,pBuffer,dwFileSize,&dwRead,NULL);
			if(dwRead == dwFileSize)
			{
                LoadSortedOpcodeBuffer( pBuffer, dwFileSize );
            }

            delete [] pBuffer;
            pBuffer = NULL;
        }
    }

    CloseHandle(hFile);

    return 1;
}

int CNikDisassembly::DumpTextOpcodeFile(_tstring strFileName)
{
    if(0 == m_vInstructionList.size()
        && (strFileName.empty()))
        return 0;

    HANDLE hFile = CreateFile(strFileName.c_str(),
                               GENERIC_WRITE,
                               FILE_SHARE_READ|FILE_SHARE_WRITE,
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
     
    if (hFile == INVALID_HANDLE_VALUE) 
        return 0;


    int nSize = 0xFEFF;     ///  UNICODE signature
    DWORD dwWritten = 0;
    _tstring strTemp;
    INSTRUCTION_OPCODE * pInstOpcode, stInstOpcode;

    ///  write UNICODE signature
    WriteFile(hFile, &nSize
            , 2, &dwWritten, NULL);

    nSize = sizeof(stInstOpcode.bOpNumber) + sizeof(stInstOpcode.strNumber)
                            + sizeof(stInstOpcode.strOpcode);

    for(int i=0;i<(int)m_vInstructionList.size();i++)
    {
        pInstOpcode = m_vInstructionList.at(i);

        strTemp = pInstOpcode->strNumber;
        strTemp += _T("\t\t\t");
        strTemp += pInstOpcode->strOpcode;
        strTemp += _T("\n");
        WriteFile(hFile, strTemp.c_str()
            , strTemp.length()*sizeof(TCHAR), &dwWritten, NULL);
    }

    CloseHandle(hFile);

    return 1;
}

int CNikDisassembly::DumpSortedOpcodeFile(_tstring strFileName)
{
    if(0 == m_vInstructionList.size()
        && (strFileName.empty()))
        return 0;

    HANDLE hFile = CreateFile(strFileName.c_str(),
                               GENERIC_WRITE,
                               FILE_SHARE_READ|FILE_SHARE_WRITE,
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
     
    if (hFile == INVALID_HANDLE_VALUE) 
        return 0;

    DWORD dwWritten = 0;
    _tstring strTemp;
    INSTRUCTION_OPCODE * pInstOpcode, stInstOpcode;

    int nSize = sizeof(stInstOpcode.bOpNumber) + sizeof(stInstOpcode.strNumber)
                            + sizeof(stInstOpcode.strOpcode);

    for(int i=0;i<(int)m_vInstructionList.size();i++)
    {
        pInstOpcode = m_vInstructionList.at(i);

        WriteFile(hFile, pInstOpcode
            , nSize, &dwWritten, NULL);
    }

    CloseHandle(hFile);

    return 1;
}

int CNikDisassembly::ParserIntructionSetFile(_tstring strFileName)
{
	CleanUp();

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

			TCHAR chLineBuffer[1024];
			chLineBuffer[1023] = 0;

			DWORD dwRead = 0;
			if(ReadFile(hFile,pBuffer,dwFileSize,&dwRead,NULL)
				&& (dwRead == dwFileSize))
			{
				int nTempsize = (int)dwFileSize;
				int nOffset = 0;
				int nLineLen = 0;
				int nTemp;
                TCHAR * pTBuffer = (TCHAR *)&pBuffer[2];
				do
				{
					nLineLen = GetLineOffset(&pTBuffer[nOffset], nTempsize);
					if((nLineLen > 0) 
						&& (nLineLen < 1024))
					{
						memcpy((BYTE *)chLineBuffer, &pTBuffer[nOffset], nLineLen*sizeof(TCHAR));
						chLineBuffer[nLineLen] = 0;

						int nPartOffset = 0;
						int nLenLenTemp = nLineLen;

						TCHAR chLineBuffer_1[1024];
						chLineBuffer_1[1023] = 0;

						int nInstValue = 0;
						INSTRUCTION_OPCODE * pInstruction_Opcode = new INSTRUCTION_OPCODE;
						if(pInstruction_Opcode)
						{
							do
							{
								nTemp = RemoveWhiteSpaces(&chLineBuffer[nPartOffset], nLenLenTemp);
								nPartOffset += nTemp;
								nLenLenTemp -= nTemp;
								nTemp = GetStringSeprator(_T('\t'), &chLineBuffer[nPartOffset], nLenLenTemp);

								memcpy((BYTE *)chLineBuffer_1, &chLineBuffer[nPartOffset], nTemp*sizeof(TCHAR));
								chLineBuffer_1[nTemp] = 0;

								if(0 == nInstValue)
								{
									//pInstruction_Opcode->strNumber = chLineBuffer_1;
                                    _tcscpy_s(pInstruction_Opcode->strNumber, 63, chLineBuffer_1);
									Fill_OpNumber(pInstruction_Opcode);
								}
								else if(1 == nInstValue)
                                {
                                    TCHAR * chTemp = _tcschr(chLineBuffer_1, _T('\r'));
                                    if(chTemp)
                                        chTemp[0] = 0;
                                    chTemp = _tcschr(chLineBuffer_1, _T('\n'));
                                    if(chTemp)
                                        chTemp[0] = 0;

                                    _tcscpy_s(pInstruction_Opcode->strOpcode, 63, chLineBuffer_1);
                                }
								//else if(2 == nInstValue)
								//	pInstruction_Opcode->strDescription = chLineBuffer_1;
								else
									break;
								nInstValue++;

								nPartOffset += nTemp;
								nLenLenTemp -= nTemp;
							}while(nLenLenTemp > 0);

							///	sort the opcode list ///
							if(m_vInstructionList.empty())
								m_vInstructionList.push_back(pInstruction_Opcode);
							else
							{
								int i;
								INSTRUCTION_OPCODE * pTemp;
								DWORD dwWord1,dwWord2;
								for(i=0;i<(int)m_vInstructionList.size();i++)
								{
									pTemp = m_vInstructionList.at(i);
									dwWord1 = 0;
									dwWord2 = 0;
									for(int j=0;j<4;j++)
									{
										if(pTemp->bOpNumber[3-j] >= 0)
											dwWord1 += pTemp->bOpNumber[3-j]<<(j*8);
										if(pInstruction_Opcode->bOpNumber[3-j] >= 0)
											dwWord2 += pInstruction_Opcode->bOpNumber[3-j]<<(j*8);
									}

									if(dwWord1 > dwWord2)
									{
										m_vInstructionList.insert(m_vInstructionList.begin()+i,pInstruction_Opcode);
										break;
									}
								}
								if(i == (int)m_vInstructionList.size())
									m_vInstructionList.push_back(pInstruction_Opcode);
							}
							///	sort the opcode list ///
						}
					}
					else
						break;								//	error line is greater than line buffer

					//nOffset += (nLineLen + 1);
                    nOffset += nLineLen;
					nTempsize = dwFileSize - (nOffset*sizeof(TCHAR));		//	set the new offset
				}while(nTempsize > 2);
			}

			delete [] pBuffer;
		}
	}

	CloseHandle(hFile);

	return 0;
}
