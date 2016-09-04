
#include "stdafx.h"
#include "DisAssembly.h"

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
                        ,_T('-'),_T('='),_T('<'),_T('>'),_T(':')
						,_T(';'),_T('\"'),_T('\''),_T('|'),_T('['),_T(']')};

TCHAR chWhitSpaces[] = {_T(' '),_T('\t')};


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
	for(int k=0;k<nNo;k++)
	{
		str = GetStringWithoutDelimiter(strTemp,nOff);
		if(-1 == nOff)
			break;
		else
			strTemp.erase(0,nOff);
	}
	return str;
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

_tstring CNikDisassembly::ProcessOpcodeStr(BYTE *pBuf, INSTRUCTION_OPCODE * pInstOpcode, int &nOpcodeBytes)
{
	_tstring str,strTemp;
	int nOff;

	nOpcodeBytes = 0;

	nOff = 0;

	{
		THREEPART * pth = (THREEPART *)&pBuf[0];

		TCHAR chBuf[32];
		//int nNoMatch = 0;
        int i;
		for(i=0;i<(int)pInstOpcode->strList.size();i++)
		{
			if(0 == i)
			{
				str = pInstOpcode->strList.at(0);
				str += _T(' ');
			}
			else
			{
				if(2 <= i)
					str += _T(", ");

				strTemp = pInstOpcode->strList.at(i);
				//if((int)strTemp.find(_T("r/m")) >= 0)
                if( R_M_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					nOpcodeBytes++;
					int nNo = (pth->mod<<3)|(pth->rm);
					if(nNo >= 24)
					{
						if((int)strTemp.find(_T("r/m8")) >= 0)
						{
                            //str += GetNoString((TCHAR *)modrm_32[nNo],3);
                            str += Reg_3[nNo-24];
						}
						else
						{
                            //str += GetNoString((TCHAR *)modrm_32[nNo],1);
                            str += Reg_1[nNo-24];
						}
					}
					else
					{
						int nPos;
						_tstring strTemp;
                        strTemp = (TCHAR *)modrm_32[nNo];

						//if((int)strTemp.find(_T("s-i-b")) >= 0)
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
								//int nOff;
								//str1 += GetStringWithoutDelimiter((TCHAR *)Reg[pth1->rm],nOff);
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
							strTemp.erase(nPos,sizeof(TCHAR)*3);
							strTemp.insert(nPos,GetFormattedStr(&pBuf[nOpcodeBytes],4));
							str += strTemp;
							nOpcodeBytes += 4;
						}
						else if((nPos=(int)strTemp.find(_T("d16"))) >= 0)
						{
							strTemp.erase(nPos,sizeof(TCHAR)*3);
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
				//else if((int)strTemp.find(_T("r32")) >= 0)
                else if( R32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
				    //str += GetStringWithoutDelimiter((TCHAR *)Reg[pth->reg],nOff);
                    str += Reg_1[pth->reg];
				}
				//else if((int)strTemp.find(_T("r8")) >= 0)
                else if( R8_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
                    //str += GetNoString((TCHAR *)Reg[pth->reg],3);
                    str += Reg_3[pth->reg];
				}
				//else if((int)strTemp.find(_T("r16")) >= 0)
                else if( R16_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
                    //str += GetNoString((TCHAR *)Reg[pth->reg],2);
                    str += Reg_2[pth->reg];
				}
				//else if((int)strTemp.find(_T("imm8")) >= 0)
                else if( IMM8_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					_stprintf_s(chBuf,32,_T("0x%X"),(char)pBuf[nOpcodeBytes]);
					str += chBuf;
					nOpcodeBytes++;
				}
				//else if((int)strTemp.find(_T("imm32")) >= 0)
                else if( IMM32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					str += GetFormattedStr(&pBuf[nOpcodeBytes],4);
					nOpcodeBytes += 4;
				}
				//else if((int)strTemp.find(_T("imm16")) >= 0)
                else if( IMM16_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					str += GetFormattedStr(&pBuf[nOpcodeBytes],2);
					nOpcodeBytes += 2;
				}
				//else if((int)strTemp.find(_T("rel8")) >= 0)
                else if( REL8_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					_stprintf_s(chBuf,32,_T("0x%X"),pBuf[nOpcodeBytes]);
					str += chBuf;
					nOpcodeBytes++;
				}
				//else if((int)strTemp.find(_T("moffs8")) >= 0)
                else if( MOFFS8_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					_stprintf_s(chBuf,32,_T("0x%X"),pBuf[nOpcodeBytes]);
					str += chBuf;
					nOpcodeBytes++;
				}
                else if( MOFFS32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				//else if(((int)strTemp.find(_T("moffs32")) >= 0)
				//	|| ((int)strTemp.find(_T("moffs16")) >= 0)
				//	|| ((int)strTemp.find(_T("moffs8")) >= 0))
				{
					str += GetFormattedStr(&pBuf[nOpcodeBytes],4);
					nOpcodeBytes += 4;
				}
				//else if((int)strTemp.find(_T("rel32")) >= 0)
                else if( REL32_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					str += GetFormattedStr(&pBuf[nOpcodeBytes],4);
					nOpcodeBytes += 4;
				}
				//else if((int)strTemp.find(_T("rel16")) >= 0)
                else if( REL16_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					str += GetFormattedStr(&pBuf[nOpcodeBytes],2);
					nOpcodeBytes += 2;
				}
				//else if((int)strTemp.find(_T("Sreg")) >= 0)
                else if( SREG_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					str += Seg_reg[pth->reg];
				}
				/*/else if((int)strTemp.find(_T("EAX")) >= 0)
                else if( EAX_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					strTemp = GetNoString(strTemp,1);
					str += strTemp;
				}
				//else if((int)strTemp.find(_T("AX")) >= 0)
                else if( AX_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					strTemp = GetNoString(strTemp,1);
					str += strTemp;
				}
				//else if((int)strTemp.find(_T("AL")) >= 0)
                else if( AL_OPCODE == pInstOpcode->bOpcodeFlag[i] )
				{
					strTemp = GetNoString(strTemp,1);
					str += strTemp;
				}*/
				else
				{
					str += strTemp;
					//str += _T(' ');
					//nNoMatch++;
				}
			}
		}
		//if(nNoMatch == i-1)
		//	str = pInstOpcode->strOpcode;

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
	int i = nOffset;
	BYTE bOp = pBuff[0],bOpp1;
	nOffset = 0;
	INSTRUCTION_OPCODE * pTemp = m_vInstructionList.at(i);
	for(;i<(int)m_vInstructionList.size();i++)
	{
        if(m_vIndexList[bOp].size())
            i = m_vIndexList[bOp].at(0);
        else
        {
            int h = bOp + 1;
            while(h < 256)
            {
                if(m_vIndexList[h].size())
                {
                    i = m_vIndexList[h].at(0);
                    break;
                }
                h++;
            }
        }

		pTemp = m_vInstructionList.at(i);
		bOpp1 = (BYTE)pTemp->bOpNumber[0];

		if(bOpp1 > bOp)
		{
			if(bOp == (BYTE)m_vInstructionList.at(i-1)->bOpNumber[0])
			{
				pTemp = m_vInstructionList.at(i-1);
				bOpp1 = bOp;
			}
		}

		if(bOpp1 >= bOp)
		{
			if(bOpp1 > bOp)
			{
				pTemp = m_vInstructionList.at(i-1);
			}

			int k,nOf=1;

			for(k=1;k<5;k++)
			{
				if((-1 != pTemp->bOpNumber[k]) && 
					(pBuff[nOf] != pTemp->bOpNumber[k]))
				{
					int bOpp2 = pBuff[nOf],j = i + 1;
					for(;j<(int)m_vInstructionList.size();j++)
					{
						INSTRUCTION_OPCODE * pTemp1 = m_vInstructionList.at(j);
						if(pTemp1->bOpNumber[k-1] == bOp)
						{
							if(pTemp1->bOpNumber[k] == bOpp2)
							{
								pTemp = m_vInstructionList.at(j);
								nOffset = k;
								break;
							}
						}
						else
							break;
					}
				}
			}

			k = i + nOffset;

			THREEPART * pth = (THREEPART *)&pBuff[1];

            while(pTemp->nSlashPresent >= 0)
            {
                if(pTemp->nSlashPresent == pth->reg)
					break;

				k++;
				if(k < (int)m_vInstructionList.size())
					pTemp = m_vInstructionList.at(k);
				else
					break;
            }

			break;
		}
	}
	return pTemp;
}

_tstring CNikDisassembly::DisOpcodeReturns(BYTE * pBuff, int &nOffset)
{
	_tstring str;
	BYTE bOp = pBuff[0];
	nOffset = 0;
	int i=0;

	/////////// special case /////////////
	if(0x90 == bOp)
	{
		nOffset++;
		return _T("NOP");
	}
	//////////////////////////////////////

	INSTRUCTION_OPCODE * pTemp = SearchNextOpcode(i,pBuff);

    int nPlus = 0;
	//nPlus = GetPlusValue(pTemp,bOp);
	//if(nPlus < 0)
	//	return str;

	i++;

    if(NULL != pTemp->strNumber[0])
	{
		str = ProcessOpcodeStr(&pBuff[i], pTemp, nPlus);
		nOffset = nPlus + i;
	}
	else
		str = pTemp->strOpcode;

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

                m_vInstructionList.push_back( pInstruct );

                {
                    int nOf = 0;
                    if((nOf = (int)(_tcschr(pInstruct->strNumber, _T('/')) - pInstruct->strNumber)) >= 0)
			        {
                        nOf = ConvertCharToNumber(pInstruct->strNumber[nOf+1]);
                        if((nOf >= 0) 
					        && (nOf < 8))
				        {
                            pInstruct->nSlashPresent = nOf;
                        }
                    }
                }

                {
                    INSTRUCTION_OPCODE * &pstInstruct = m_vInstructionList.at(i);
                    int nOff = 0,nOffImm = 0, ii = 0;
                    _tstring strTemp, strOp = pstInstruct->strOpcode;

                    do
                    {
                        strTemp = GetStringWithoutDelimiter(&strOp.data()[nOff],nOffImm);
                        if(false == strTemp.empty())
                        {
                            if((int)strTemp.find(_T("r/m")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R_M_OPCODE;
                            }
                            else if((int)strTemp.find(_T("r/m8")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R_M8_OPCODE;
                            }
                            else if((int)strTemp.find(_T("d8")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = D8_OPCODE;
                            }
                            else if((int)strTemp.find(_T("d32")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = D32_OPCODE;
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
                            }
                            else if((int)strTemp.find(_T("imm32")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = IMM32_OPCODE;
                            }
                            else if((int)strTemp.find(_T("rel8")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = REL8_OPCODE;
                            }
                            else if((int)strTemp.find(_T("rel32")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = REL32_OPCODE;
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
                            }
                            else if((int)strTemp.find(_T("d16")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = D16_OPCODE;
                            }
                            else if((int)strTemp.find(_T("r16")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = R16_OPCODE;
                            }
                            else if((int)strTemp.find(_T("imm16")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = IMM16_OPCODE;
                            }
                            else if((int)strTemp.find(_T("rel16")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = REL16_OPCODE;
                            }
                            else if((int)strTemp.find(_T("moffs8")) >= 0)
                            {
                                pstInstruct->bOpcodeFlag[ii] = MOFFS8_OPCODE;
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

                m_vIndexList[((INSTRUCTION_OPCODE *)pInstOpcode)->bOpNumber[0]].push_back(i);

                pInstOpcode += nSizeStruct;
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


    int nSize = 0XFEFF;     ///  UNICODE signature
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

									if(dwWord1 >= dwWord2)
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

