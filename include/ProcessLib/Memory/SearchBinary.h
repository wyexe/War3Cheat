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
	public:
		DWORD	FindAddr(_In_ LPCSTR lpszCode, _In_ int nOffset, _In_ int nOrderNum, _In_ LPCWSTR lpszModule);
		DWORD	FindCALL(_In_ LPCSTR lpszCode, _In_ int nOffset, _In_ int nMov, _In_ int nOrderNum, _In_ LPCWSTR lpszModule);
		DWORD	FindBase(_In_ LPCSTR lpszCode, _In_ int nOffset, _In_ int nMov, _In_ int nOrderNum, _In_ LPCWSTR lpszModule, DWORD dwAddrLen = 0xFFFFFFFF);
		DWORD	FindBase_ByCALL(_In_ LPCSTR lpszCode, _In_ int nOffset, _In_ int nMov, _In_ int nOrderNum, _In_ LPCWSTR lpszModule, _In_ int nBaseOffset, _In_opt_ DWORD dwAddrLen = 0xFFFFFFFF);
	private:
		BOOL	SearchBase(_In_ LPCSTR szCode, _Out_ DWORD * pArray, _Out_ UINT& puLen, _In_ LPCWSTR lpszModule);
		DWORD	GetImageSize(_In_ DWORD dwImageBase);
		BOOL	CL_sunday(_In_ DWORD* pKey, _In_ UINT uKeyLen, _In_ BYTE* pCode, _In_ UINT uCodeLen, _Out_ std::vector<int>& vlst);
		int		GetWord_By_Char(_In_ BYTE dwWord, _In_ DWORD* pKey, _In_ UINT uKeyLen);
		BOOL	CompCode(_In_ const DWORD * pCode, _In_ const BYTE * pMem, _In_ UINT uLen);
#endif // _WIN64
	};
}

#endif // !__LIBTOOLS_PROCESSLIB_MEMORY_SEARCHBINARY_H__
