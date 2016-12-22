﻿/*
 * (c) Copyright Ascensio System SIA 2010-2016
 *
 * This program is a free software product. You can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License (AGPL)
 * version 3 as published by the Free Software Foundation. In accordance with
 * Section 7(a) of the GNU AGPL its Section 15 shall be amended to the effect
 * that Ascensio System SIA expressly excludes the warranty of non-infringement
 * of any third-party rights.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE. For
 * details, see the GNU AGPL at: http://www.gnu.org/licenses/agpl-3.0.html
 *
 * You can contact Ascensio System SIA at Lubanas st. 125a-25, Riga, Latvia,
 * EU, LV-1021.
 *
 * The  interactive user interfaces in modified source and object code versions
 * of the Program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU AGPL version 3.
 *
 * Pursuant to Section 7(b) of the License you must retain the original Product
 * logo when distributing the program. Pursuant to Section 7(e) we decline to
 * grant you any rights under trademark law for use of our trademarks.
 *
 * All the Product's GUI elements, including illustrations and icon sets, as
 * well as technical writing content are licensed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International. See the License
 * terms at http://creativecommons.org/licenses/by-sa/4.0/legalcode
 *
 */
#pragma once

#include "../XML/xmlutils.h"
#include "../../../../DesktopEditor/common/File.h"

#include "../../../3dParty/pole/pole.h"
#include "../Base/unicode_util.h"
#include "../Base/Types_32.h"

//по аналогии с /DesktopEditor/common/File.h только для CString

#ifndef COINIT_MULTITHREADED
	#define COINIT_MULTITHREADED 0				//for win64 .... oO
#endif

class CFile 
{
private:
	HRESULT _Open(const std::wstring& strFileName, bool bOpen = false, bool bCreate = false, bool bReadWrite = false)
	{
		HRESULT hRes = S_OK;
		CloseFile();

#if defined(_WIN32) || defined(_WIN32_WCE) ||defined(_WIN64)
		wchar_t* pModeOpen;
		wchar_t* pModeCreate;
		if(bReadWrite)
		{
			pModeOpen = L"rb+";
			pModeCreate = L"wb+";
		}
		else
		{
			pModeOpen = L"rb";
			pModeCreate = L"wb";
		}
		if(NULL == m_pFile && bOpen)
			m_pFile = _wfopen(strFileName.c_str(), pModeOpen);
		if(NULL == m_pFile && bCreate)
			m_pFile = _wfopen(strFileName.c_str(), pModeCreate);
#else
		BYTE* pUtf8 = NULL;
		LONG lLen = 0;
		NSFile::CUtf8Converter::GetUtf8StringFromUnicode(strFileName.c_str(), strFileName.length(), pUtf8, lLen, false);
		char* pModeOpen;
		char* pModeCreate;
		if(bReadWrite)
		{
			pModeOpen = "rb+";
			pModeCreate = "wb+";
		}
		else
		{
			pModeOpen = "rb";
			pModeCreate = "wb";
		}
		if(NULL == m_pFile && bOpen)
			m_pFile = fopen((char*)pUtf8, pModeOpen);
		if(NULL == m_pFile && bCreate)
			m_pFile = fopen((char*)pUtf8, pModeCreate);
		RELEASEARRAYOBJECTS(pUtf8);
#endif
		if (NULL == m_pFile)
            return S_FALSE;

		fseek(m_pFile, 0, SEEK_END);
		m_lFileSize = ftell(m_pFile);
		fseek(m_pFile, 0, SEEK_SET);

		m_lFilePosition = 0;

		if (0 < strFileName.length())
		{
			if (((wchar_t)'/') == strFileName[strFileName.length() - 1])
				m_lFileSize = 0x7FFFFFFF;
		}

		unsigned int err = 0x7FFFFFFF;
		unsigned int cur = (unsigned int)m_lFileSize;
		if (err == cur)
		{
			CloseFile();
			return S_FALSE;
		}

		return hRes;
	}
public:
	CFile() 
	{
		m_pFile = NULL;
		m_lFilePosition = 0;
		m_lFileSize = 0;
	}

	virtual ~CFile()
	{
		CloseFile();
	}
	
	HRESULT OpenOrCreate(std::wstring strFileName, bool bOnlyOpen = false, bool bReadWrite = false)
	{
		return _Open(strFileName, true, true, true);
	}
	virtual HRESULT OpenFile(std::wstring FileName)
	{	
		return _Open(FileName, true, false, false);
	}

	virtual HRESULT OpenFileRW(std::wstring FileName)
	{	
		return _Open(FileName, true, false, true);
	}

	HRESULT ReadFile(BYTE* pData, DWORD nBytesToRead)
	{
		if (!m_pFile)
			return S_FALSE;

		SetPosition(m_lFilePosition);
		DWORD dwSizeRead = (DWORD)fread((void*)pData, 1, nBytesToRead, m_pFile);
		m_lFilePosition += dwSizeRead; 
		return S_OK;
	}

	HRESULT ReadFile2(BYTE* pData, DWORD nBytesToRead)
	{
		HRESULT hRes = ReadFile(pData, nBytesToRead);
		//reverse bytes
		for (size_t index = 0; index < nBytesToRead / 2; ++index)
		{
			BYTE temp = pData[index];
			pData[index] = pData[nBytesToRead - index - 1];
			pData[nBytesToRead - index - 1] = temp;
		}
		return S_OK;
	}
	HRESULT ReadFile3(void* pData, DWORD nBytesToRead)
	{
		return ReadFile((BYTE*)pData, nBytesToRead);
	}

	HRESULT WriteFile(void* pData, DWORD nBytesToWrite)
	{
		if (!m_pFile)
			return S_FALSE;

		size_t nCountWrite = fwrite((void*)pData, 1, nBytesToWrite, m_pFile);
		m_lFilePosition += nBytesToWrite; 
		return S_OK;
	}

	HRESULT WriteFile2(void* pData, DWORD nBytesToWrite)
	{
		if (!m_pFile)
			return S_FALSE;

		BYTE* mem = new BYTE[nBytesToWrite];
		memcpy(mem, pData, nBytesToWrite);

		for (size_t index = 0; index < nBytesToWrite / 2; ++index)
		{
			BYTE temp = mem[index];
			mem[index] = mem[nBytesToWrite - index - 1];
			mem[nBytesToWrite - index - 1] = temp;
		}

		return WriteFile(mem, nBytesToWrite);
	}

	HRESULT CreateFile(std::wstring strFileName)
	{
		return _Open(strFileName, false, true, true);
	}
	HRESULT SetPosition( ULONG64 nPos )
	{	
		if (m_pFile && nPos <= (ULONG)m_lFileSize)
		{
			m_lFilePosition = (long)nPos;
			fseek(m_pFile, m_lFilePosition, SEEK_SET);
			return S_OK;
		}
		else 
		{
			return !m_pFile ? S_FALSE : S_OK;
		}
	}
	LONG64  GetPosition()
	{
		return m_lFilePosition;
	}
	HRESULT SkipBytes(ULONG64 nCount)
	{
		return SetPosition(m_lFilePosition + nCount);
	}

	HRESULT CloseFile()
	{
		m_lFilePosition = 0;
		m_lFileSize = 0;

		if (m_pFile != NULL)
		{
			fclose(m_pFile);
			m_pFile = NULL;		
		}
		return S_OK;
	}

	ULONG64 GetFileSize()
	{
		return m_lFileSize;
	}

	HRESULT WriteReserved(DWORD dwCount)
	{
		BYTE* buf = new BYTE[dwCount];
		memset(buf, 0, (size_t)dwCount);
		HRESULT hr = WriteFile(buf, dwCount);
		RELEASEARRAYOBJECTS(buf);
		return hr;
	}
	HRESULT WriteReserved2(DWORD dwCount)
	{
		BYTE* buf = new BYTE[dwCount];
		memset(buf, 0xFF, (size_t)dwCount);
		HRESULT hr = WriteFile(buf, dwCount);
		RELEASEARRAYOBJECTS(buf);
		return hr;
	}
	HRESULT WriteReservedTo(DWORD dwPoint)
	{
		if ((DWORD)m_lFilePosition >= dwPoint)
			return S_OK;

		DWORD dwCount = dwPoint - (DWORD)m_lFilePosition;
		BYTE* buf = new BYTE[dwCount];
		memset(buf, 0, (size_t)dwCount);
		HRESULT hr = WriteFile(buf, dwCount);
		RELEASEARRAYOBJECTS(buf);
		return hr;
	}
	HRESULT SkipReservedTo(DWORD dwPoint)
	{
		if ((DWORD)m_lFilePosition >= dwPoint)
			return S_OK;

		DWORD dwCount = dwPoint - (DWORD)m_lFilePosition;
		return SkipBytes(dwCount);
	}

	LONG GetProgress()
	{
		if (0 >= m_lFileSize)
			return -1;

		double dVal = (double)(100 * m_lFilePosition);
		LONG lProgress = (LONG)(dVal / m_lFileSize);
		return lProgress;
	}

	void WriteStringUTF8(const std::wstring& strXml)
	{
		BYTE* pData = NULL;
		LONG lLen = 0;

		NSFile::CUtf8Converter::GetUtf8StringFromUnicode(strXml.c_str(), (LONG)strXml.length(), pData, lLen, false);

		WriteFile(pData, lLen);

		RELEASEARRAYOBJECTS(pData);
	}
protected:
	FILE* m_pFile;

	long m_lFilePosition;
	long m_lFileSize;
};
namespace StreamUtils
{
	static BYTE ReadBYTE(POLE::Stream* pStream)
	{
		if (pStream == NULL) return 0;
		
		BYTE lMem = 0;
		ULONG lReadByte = 0;
		
		lReadByte = pStream->read(&lMem, 1);
		if (lReadByte < 1)
		{
			lMem = 0;
		}
		return lMem;
	}
	static WORD ReadWORD(POLE::Stream* pStream)
	{
		if (pStream == NULL) return 0;

		WORD lWord = 0;
		BYTE pMem[2];
		ULONG lReadByte = 0;
	
		lReadByte = pStream->read(pMem, 2);
		if (lReadByte == 2)
		{
			lWord = ((pMem[1] << 8) | pMem[0]);
		}
		return lWord;
	}
	static DWORD ReadDWORD(POLE::Stream* pStream)
	{
		if (pStream == NULL) return 0;

		DWORD lDWord = 0;
		BYTE pMem[4];
		ULONG lReadByte = 0;
		lReadByte = (ULONG)pStream->read(pMem, 4);
		
//#if defined(_DEBUG) && (defined(_WIN32) || defined(_WIN64))
//		ATLASSERT(4 == lReadByte);
//#endif

		if (lReadByte == 4)
		{
			lDWord = ((pMem[3] << 24) | (pMem[2] << 16) | (pMem[1] << 8) | pMem[0]);
		}
        return 0xFFFFFFFF & lDWord;
	}
	static SHORT ReadSHORT(POLE::Stream* pStream)
	{
        return (short)ReadWORD(pStream);
	}
	static LONG ReadLONG(POLE::Stream* pStream)
	{
        return (_INT32)ReadDWORD(pStream);
	}
	
	static FLOAT ReadFLOAT( POLE::Stream* pStream)
	{
		if (pStream == NULL) return 0;

		FLOAT Value = 0.0f;
		pStream->read ((unsigned char*) &Value, sizeof (FLOAT));
		return Value;
	}

	static std::string ReadStringA(POLE::Stream* pStream, LONG lLen)
	{
		if (pStream == NULL) return ("");

		char* pData = new char[lLen + 1];
		ULONG lReadByte = 0;
		
		lReadByte = (ULONG)pStream->read((unsigned char*)pData, lLen);

		pData[lLen] = 0;

		std::string str(pData);

		delete[] pData;
		return str;
	}
	static std::wstring ReadStringW(POLE::Stream* pStream, LONG lLen)
	{
		if (pStream == NULL) return _T("");
	
		unsigned char* pData = new unsigned char[2 * (lLen + 1)];
		memset (pData, 0, 2 * (lLen + 1));

		ULONG lReadByte = 0;		
		lReadByte = (ULONG)pStream->read(pData, 2 * lLen);

		if (sizeof(wchar_t) == 4)
		{
			ConversionResult eUnicodeConversionResult;
			UTF32 *pStrUtf32 = new UTF32 [lLen + 1];
            pStrUtf32[lLen] = 0 ;

			const	UTF16 *pStrUtf16_Conv = (const UTF16 *) pData;
					UTF32 *pStrUtf32_Conv =                 pStrUtf32;

	eUnicodeConversionResult = ConvertUTF16toUTF32 ( &pStrUtf16_Conv
                                               , &pStrUtf16_Conv[lLen]
                                               , &pStrUtf32_Conv
                                               , &pStrUtf32 [lLen]
                                               , strictConversion);

			if (conversionOK != eUnicodeConversionResult)
			{
				delete [] pStrUtf32;
                return _T("");
            }
			std::wstring res((wchar_t*)pStrUtf32, lLen);
			if (pStrUtf32) delete [] pStrUtf32;
            return res;
        }
		else
		{
			std::wstring str((wchar_t*)pData);
			delete[] pData;
			return str;
		}

	}
	static std::string ConvertCStringWToCStringA(std::wstring& strW)
	{
		std::string str_a(strW.begin(), strW.end());

		return str_a;
	}
	static void StreamSeek(long lOffset, POLE::Stream* pStream)
	{
		if (pStream == NULL) return;

		pStream->seek(lOffset); 
	}
	static void StreamPosition(long& lPosition, POLE::Stream* pStream)
	{
		if (pStream == NULL) return ;

		lPosition = (LONG)pStream->tell();
	}
	static void StreamSkip(long lCount, POLE::Stream* pStream)
	{
		if (pStream == NULL) return;

		pStream->seek(pStream->tell() + lCount); 
	}
	static void StreamSkipBack(long lCount, POLE::Stream* pStream)
	{
		if (pStream == NULL) return;

		pStream->seek(pStream->tell()-lCount); 
	}
}

#if defined(_WIN32) || defined(_WIN64)
namespace CDirectory
{
	static std::wstring GetFolderName(std::wstring strFolderPath)
	{
		int n1 = strFolderPath.rfind('\\');
		if (-1 == n1)
			return _T("");

		return strFolderPath.substr(n1 + 1);
	}
	static std::wstring GetFolderPath(std::wstring strFolderPath)
	{
		int n1 = strFolderPath.rfind('\\');
		if (-1 == n1)
			return _T("");

		return strFolderPath.substr(0, n1);
	}
    static bool OpenFile(std::wstring strFolderPath, std::wstring strFileName, CFile* pFile)
	{
        std::wstring strFile = strFolderPath  + FILE_SEPARATOR_STR + strFileName;
		return (S_OK == pFile->OpenFile(strFile));
	}
    static bool CreateFile(std::wstring strFolderPath, std::wstring strFileName, CFile* pFile)
	{
        std::wstring strFile = strFolderPath + FILE_SEPARATOR_STR + strFileName;
		return (S_OK == pFile->CreateFile(strFile));
	}
    static bool CreateDirectory(std::wstring strFolderPathRoot, std::wstring strFolderName)
	{
        std::wstring strFolder = strFolderPathRoot + FILE_SEPARATOR_STR + strFolderName;
		return (bool)CreateDirectory(strFolder, NULL);
	}
    static bool CreateDirectory(std::wstring strFolderPath)
	{
		return (bool)CreateDirectory(strFolderPath, NULL);
	}
    static bool DeleteFile (std::wstring strFileName)
    {
        ::DeleteFile (strFileName.c_str());
        return true;
    }

    static bool MoveFile(std::wstring strExists, std::wstring strNew, LPVOID lpFunc, LPVOID lpData)
	{
#if (_WIN32_WINNT >= 0x0500) && !defined (_WIN64)
		return (bool)MoveFileWithProgress(strExists.c_str(), strNew.c_str(), (LPPROGRESS_ROUTINE)lpFunc, lpData, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH); 
#else
		return (bool)MoveFileEx(strExists.c_str(), strNew.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH); 
#endif
	}

    static bool CopyFile(std::wstring strExists, std::wstring strNew, LPVOID lpFunc, LPVOID lpData)
	{
		DeleteFile(strNew);
//#if (_WIN32_WINNT >= 0x0500) && !defined (_WIN64)
//		return (bool)CopyFile(strExists.c_str(), strNew.c_str(), true); 
//#else
		return (bool)CopyFileEx(strExists.c_str(), strNew.c_str(), (LPPROGRESS_ROUTINE)lpFunc, lpData, FALSE, 0); 
//#endif
	}

	static std::wstring GetUnder(std::wstring strFolderPathRoot, std::wstring strFolderName)
	{
		std::wstring strFolder = strFolderPathRoot + L"\\" + strFolderName;
		return strFolder;
	}

	static std::wstring GetFileName(std::wstring strFullName)
	{
		size_t nStart = strFullName.rfind(L"\\");
		std::wstring strName = strFullName.substr(nStart + 1);
		return strName;
	}

	static std::wstring BYTEArrayToString2(USHORT* arr, size_t nCount)
	{
		std::wstring str;
		for (size_t index = 0; index < nCount; ++index)
		{
			if ('\0' != (char)(arr[index]))
				str += (char)(arr[index]);
		}
		if (str.length() == 0)
			str = _T("0");
		return str;
	}

	static std::wstring ToString(DWORD val)
	{
		return std::to_wstring(val);
	}

	static std::wstring ToString(UINT64 val, bool bInit)
	{
		std::wstring strCoarse = ToString((DWORD)(val >> 32));
		if (_T("0") != strCoarse)
		{
			return strCoarse + ToString((DWORD)val);
		}
		
		return ToString((DWORD)val);
	}

	static UINT64 GetUINT64(std::wstring strVal)
	{
		UINT64 nRet = 0;
		int nLen = (int)strVal.length();
		while (nLen > 0)
		{
			int nDig = XmlUtils::GetDigit(strVal[0]);
			nRet *= 10;
			nRet += nDig;
			strVal.erase(0);
			--nLen;
		}
		return nRet;
	}
	static UINT GetUINT(std::wstring strVal)
	{
		return (UINT)GetUINT64(strVal);
	}

	static void SaveToFile(std::wstring strFileName, std::wstring strXml)
	{
        int nLength = (int)strXml.length();

		std::string saStr; saStr.reserve(nLength*3 + 1);
		
#ifdef UNICODE
		// Encoding Unicode to UTF-8
		WideCharToMultiByte(CP_UTF8, 0, strXml.c_str(), nLength + 1, (LPSTR)saStr.c_str(), nLength * 3, NULL, NULL); 
#else
		wchar_t* pWStr = new wchar_t[nLength + 1];
		if (!pWStr)
			return;

		// set end string
		pWStr[nLength] = 0;

		// Encoding ASCII to Unicode
        MultiByteToWideChar(CP_ACP, 0, strXml, nLength, pWStr, nLength);

        int nLengthW = (int)wcslen(pWStr);

		// Encoding Unicode to UTF-8
        WideCharToMultiByte(CP_UTF8, 0, pWStr, nLengthW + 1, saStr.GetBuffer(nLengthW*3 + 1), nLengthW*3, NULL, NULL);
		saStr.ReleaseBuffer();

	    delete[] pWStr;
#endif
		
		CFile oFile;
		oFile.CreateFile(strFileName);
		oFile.WriteFile((void*)saStr.c_str(), saStr.length());
		oFile.CloseFile();
	}

	static void SaveToFile2(std::wstring strFileName, std::string strVal)
	{
		CFile oFile;
		HRESULT hr = oFile.OpenFileRW(strFileName);

		if (S_OK != hr)
			oFile.CreateFile(strFileName);

		oFile.SkipBytes(oFile.GetFileSize());
		oFile.WriteFile((void*)strVal.c_str(), strVal.length());
		oFile.CloseFile();
	}
}

#else

#include <iostream>
#include <fstream>  // instream
#include <unistd.h> // unlink

#include "../../../../DesktopEditor/common/Types.h"
#include "../../../../DesktopEditor/common/File.h"

namespace CDirectory
{

    static void SaveToFile(std::wstring strFileName, std::wstring strXml)
    {
        std::string aContentUtf8 = stringWstingToUtf8String (strXml);
        std::string aFileNameUtf8 = stringWstingToUtf8String (strFileName);

        FILE *pFile = fopen(aFileNameUtf8.c_str(), "wb");

        if (NULL == pFile)
        {
            throw std::exception();
        }
        fwrite (aContentUtf8.c_str() , sizeof(char), aContentUtf8.size(), pFile);
        fclose (pFile);
    }
    static bool DeleteFile (std::wstring strFileName)
    {
        std::string aFileNameUtf8 = stringWstingToUtf8String (strFileName);
        return 0 == unlink (aFileNameUtf8.c_str());
    }
    static std::wstring ToString(DWORD val)
    {
        return std::to_wstring(val);
    }

    static bool CopyFile (std::wstring strExists, std::wstring strNew, LPVOID lpFunc = NULL, LPVOID lpData = NULL)
    {
        bool bRes = true;
        try
        {
            std::string stdstrFrom = stringWstingToUtf8String(strExists);
            std::string stdstrTo = stringWstingToUtf8String(strNew);
            std::ifstream  src (stdstrFrom.c_str(), std::ios::binary);
            std::ofstream  dst (stdstrTo.c_str(),   std::ios::binary);

            dst << src.rdbuf();
        }
        catch (...)
        {
            bRes = false;
        }
        return bRes;
    }
}
#endif

namespace CDirectory
{
    static void WriteValueToNode(std::wstring strName, DWORD value, XmlUtils::CXmlWriter* pWriter)
    {
        pWriter->WriteNodeBegin(strName);
        pWriter->WriteString(CDirectory::ToString(value));
        pWriter->WriteNodeEnd(strName);
    }
    static void WriteValueToNode(std::wstring strName, LONG value, XmlUtils::CXmlWriter* pWriter)
    {
        pWriter->WriteNodeBegin(strName);

		std::wstring strLONG = std::to_wstring(value);

		pWriter->WriteString(strLONG);
        pWriter->WriteNodeEnd(strName);
    }
    static void WriteValueToNode(std::wstring strName, std::wstring value, XmlUtils::CXmlWriter* pWriter)
    {
        pWriter->WriteNodeBegin(strName);
        pWriter->WriteString(value);
        pWriter->WriteNodeEnd(strName);
    }
    static void WriteValueToNode(std::wstring strName, WCHAR value, XmlUtils::CXmlWriter* pWriter)
    {
        wchar_t str_arr[2]={};
        str_arr[0] = value;
        std::wstring str(str_arr);

        pWriter->WriteNodeBegin(strName);
        pWriter->WriteString(str);
        pWriter->WriteNodeEnd(strName);
    }
    static void WriteValueToNode(std::wstring strName, bool value, XmlUtils::CXmlWriter* pWriter)
    {
        pWriter->WriteNodeBegin(strName);
        std::wstring str = (true == value) ? _T("1") : _T("0");
        pWriter->WriteString(str);
        pWriter->WriteNodeEnd(strName);
    }

    static double FixedPointToDouble(DWORD point)
    {
        double dVal = (double)(point % 65536) / 65536;
        dVal += (point / 65536);
        return dVal;
    }
    static LONG NormFixedPoint(DWORD point, LONG base)
    {
        return (LONG)(FixedPointToDouble(point) * base);
    }


    static std::wstring BYTEArrayToString(BYTE* arr, size_t nCount)
    {
        std::wstring str;
        for (size_t index = 0; index < nCount; ++index)
        {
            if ('\0' != (char)(arr[index]))
                str += (char)(arr[index]);
        }
        if (str.length() == 0)
            str = _T("0");
        return str;
    }

    static std::wstring BYTEArrayToStringW(BYTE* arr, size_t nCount)
    {
        std::wstring str;
        wchar_t* pArr = (wchar_t*)arr;
        size_t nCountNew = nCount / 2;
        for (size_t index = 0; index < nCountNew; ++index)
        {
            str += pArr[index];
        }
        if (str.length() == 0)
            str = _T("0");
        return str;
    }
}
