#ifndef __LIBTOOLS_LOGLIB_LOGEXPRESSIONPELOADER_H__
#define __LIBTOOLS_LOGLIB_LOGEXPRESSIONPELOADER_H__

#include <Windows.h>
#include <string>
#include <vector>

namespace libTools
{
	class CLogExpressionPeLoader
	{
	public:
		struct ExportTable
		{
			DWORD64			dwOrdinal = NULL;
			DWORD64			dwMethodPtr = NULL;
			std::wstring	wsFunName;
		};

		struct ImportDLLContent
		{
			std::wstring wsAPIName;
			DWORD64		 dwFuncRVA = NULL;
		};

		struct ImportTable
		{
			std::wstring wsDLLName;
			std::vector<ImportDLLContent> VecTable;
		};
	public:
		CLogExpressionPeLoader() = default;
		~CLogExpressionPeLoader();

		static VOID PeLoader(_In_ CONST std::vector<std::wstring>& VecParam);

		// Set Applocation binary Code
		BOOL SetFileContent(_In_ CONST std::wstring& wsFilePath);
		BOOL SetFileContent(_In_ LPVOID pvCode, _In_ UINT uSize);

		// 导入表
		BOOL GetVecImportTable(_Out_ std::vector<ImportTable>& Vec) CONST;

		// 导出表
		BOOL GetVecExportTable(_Out_ std::vector<ExportTable>& Vec) CONST;
	private:
		DWORD						GetImageBase() CONST;

		CONST IMAGE_DOS_HEADER*		GetDosHeader() CONST;

		CONST IMAGE_NT_HEADERS*		GetNtHeaderX86() CONST;

		CONST IMAGE_NT_HEADERS64*	GetNtHeaderX64() CONST;

		BOOL						IsX86() CONST;

		BOOL						IsValidPE() CONST;

		DWORD						RvaToOffsetX86(_In_ DWORD dwRva) CONST;

		DWORD64						RvaToOffsetX64(_In_ DWORD64 dwRva) CONST;

		// 导入表 x86
		BOOL						GetVecImportTableX86(_Out_ std::vector<ImportTable>& Vec) CONST;

		// 导入表 x64
		BOOL						GetVecImportTableX64(_Out_ std::vector<ImportTable>& Vec) CONST;

		// 导出表 x86
		BOOL						GetVecExportTableX86(_Out_ std::vector<ExportTable>& Vec) CONST;

		// 导出表 x64
		BOOL						GetVecExportTableX64(_Out_ std::vector<ExportTable>& Vec) CONST;
	private:
		BOOL   _bAlloc = FALSE;
		LPVOID _pvFileContent = nullptr;
		SIZE_T _uFileSize = NULL;
	};
}

#endif // !__LIBTOOLS_LOGLIB_LOGEXPRESSIONPELOADER_H__
