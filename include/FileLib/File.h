#ifndef __LIBTOOLS_FILELIB_FILE_H__
#define __LIBTOOLS_FILELIB_FILE_H__

#include <Windows.h>
#include <string>

namespace libTools
{
	class CFile
	{
	public:
		CFile() = default;
		~CFile() = default;

	public:
		// Create Directory
		static BOOL WINAPI CreateMyDirectory(IN LPCWSTR pwchText, BOOL bShowErr = FALSE);

		// Read txt Text in Unicode Encoding
		static BOOL WINAPI ReadUnicodeFile(_In_ CONST std::wstring& cwsPath, _Out_ std::wstring& cwsContent);

		// Read txt Text in ASCII Encoding
		static BOOL WINAPI ReadASCIIFile(_In_ CONST std::wstring& cwsPath, _Out_ std::wstring& cwsContent);

		// Write txt Text in Unicode Encoding(Overwrite)
		static BOOL WINAPI WriteUnicodeFile(_In_ CONST std::wstring& cwsPath, _In_ CONST std::wstring& cwsContent);

		// Write txt Text in ASCII Encoding(Overwrite)
		static BOOL WINAPI WriteASCIIFile(_In_ CONST std::wstring& cwsPath, _In_ CONST std::wstring& cwsContent);

		// Write BYTE Content(Overwrite)
		static BOOL WINAPI WriteFile(_In_ CONST std::wstring& cwsPath, _In_ CONST BYTE* Buffer, _In_ UINT uSize);

		// Append BYTE Content
		static BOOL WINAPI AppendFile(_In_ CONST std::wstring& cwsPath, _In_ CONST BYTE* Buffer, _In_ UINT uSize);

		// Append txt Text in Unicode Encoding
		static BOOL WINAPI AppendUnicodeFile(_In_ CONST std::wstring& cwsPath, _In_ CONST std::wstring& cwsContent);

		// Create Text File in Unicode Encoding(Overwrite)
		static BOOL WINAPI CreateUnicodeTextFile(_In_ CONST std::wstring& cwsPath);

		// Create Text File in ASCII Encoding(Overwrite)
		static BOOL WINAPI CreateASCIITextFile(_In_ CONST std::wstring& cwsPath);

		// Clear ALL Content
		static BOOL WINAPI ClearFileContent(_In_ CONST std::wstring& cwsPath);

		// Read File Content(have to use ::VirtualFree(lpFileContent,0,MEM_RELEASE) when free memory~)
		static BOOL WINAPI ReadFileContent(_In_ CONST std::wstring& wsPath, _Out_ LPVOID& lpFileContent, _Out_ SIZE_T& uSize);

		// Param1 = "\\Config.ini"
		static BOOL WINAPI ReadUnicodeConfig(_In_ CONST std::wstring& wsConfigPath, _In_ CONST std::wstring& wsConfigText, _In_ CONST std::wstring& wsConfigKey, _Out_ std::wstring& wsConfigValue);

		// File Is Exist
		static BOOL	WINAPI FileExist(_In_ CONST std::wstring& wsPath);

		// Get File Size
		static BOOL WINAPI ReadAsciiFileLen(_In_ CONST std::wstring& cwsPath, _Out_ ULONG& ulFileLen);

		// Directory is Exist
		static BOOL WINAPI DirectoryExist(_In_ CONST std::wstring& wsPath);
	};

}

#endif // !__LIBTOOLS_FILELIB_FILE_H__
