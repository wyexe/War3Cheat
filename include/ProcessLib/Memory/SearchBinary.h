#ifndef __LIBTOOLS_PROCESSLIB_MEMORY_SEARCHBINARY_H__
#define __LIBTOOLS_PROCESSLIB_MEMORY_SEARCHBINARY_H__

#include <Windows.h>
#include <vector>

namespace libTools
{
	class CSearchBinary
	{
	public:
		CSearchBinary() = default;
		~CSearchBinary() = default;

#ifdef _WIN64
#else
		DWORD	FindAddr(LPCSTR lpszCode, int nOffset, int nOrderNum, LPCWSTR lpszModule);
		DWORD	FindCALL(LPCSTR lpszCode, int nOffset, DWORD dwModuleAddr, int nMov, int nOrderNum, LPCWSTR lpszModule);
		DWORD	FindBase(LPCSTR lpszCode, int nOffset, int nMov, int nOrderNum, LPCWSTR lpszModule, DWORD dwAddrLen = 0xFFFFFFFF);
		DWORD	FindBase_ByCALL(LPCSTR lpszCode, int nOffset, DWORD dwModuleAddr, int nMov, int nOrderNum, LPCWSTR lpszModule, int nBaseOffset, DWORD dwAddrLen = 0xFFFFFFFF);
		BOOL	SearchBase(LPCSTR szCode, DWORD * pArray, UINT& puLen, LPCWSTR lpszModule);
		DWORD	GetImageSize(HMODULE hm);
		BOOL	CL_sunday(DWORD* pKey, UINT uKeyLen, BYTE* pCode, UINT uCodeLen, std::vector<int>& vlst);
		int		GetWord_By_Char(BYTE dwWord, DWORD* pKey, UINT uKeyLen);
		BOOL	CompCode(const DWORD * pCode, const BYTE * pMem, UINT uLen);
#endif // _WIN64

	private:

	};
}

#endif // !__LIBTOOLS_PROCESSLIB_MEMORY_SEARCHBINARY_H__
