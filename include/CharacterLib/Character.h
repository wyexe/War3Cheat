#ifndef __LIBTOOLS_CHARACTERLIB_CHARACTER_H__
#define __LIBTOOLS_CHARACTERLIB_CHARACTER_H__

#include <vector>
#include <include/ExceptionLib/Exception.h>



namespace libTools
{
	class CCharacter
	{
	public:
		CCharacter() = default;
		~CCharacter() = default;

		template<typename T>
		static void strcpy_my(_In_ T * dest, _In_ CONST T * src, _In_opt_ SIZE_T MaxSize = 1024)
		{
			CException::InvokeAction(__FUNCTIONW__, [&]
			{
				size_t len = strlen_my(src);
				for (size_t i = 0; i < len && i < MaxSize; ++i)
				{
					*(dest + i) = *(src + i);
				}

				*(dest + len) = '\0';
			});
		}

		template<typename T>
		static size_t strlen_my(_In_ CONST T * dest)
		{
			return CException::InvokeFunc<size_t>(__FUNCTIONW__, [=]
			{
				size_t i = 0;
				for (; *(dest + i) != '\0'; ++i);
				return i;
			});
		}

		template<typename T>
		static bool strcmp_my(_In_ CONST T * dest, _In_ CONST T * src)
		{
			if (strlen_my(dest) != strlen_my(src))
			{
				return false;
			}

			return CException::InvokeFunc<bool>(__FUNCTIONW__, [=]
			{
				while (*dest != '\0')
				{
					if (*dest != *src)
					{
						return false;
					}

					dest += 1;
					src += 1;
				}

				return true;
			});
		}

		template<typename T>
		static std::basic_string<T>& Trim(_In_ _Out_ std::basic_string<T>& s)
		{
			if (s.empty())
				return s;

			CException::InvokeAction(__FUNCTIONW__, [&]
			{
				std::basic_string<T>::iterator c;
				for (c = s.begin(); c != s.end() && iswspace(*c++);); s.erase(s.begin(), --c);
				for (c = s.end(); c != s.begin() && iswspace(*--c);); s.erase(++c, s.end());
			});

			return s;
		}

		template<typename T>
		static void GetRemoveLeft(_In_ CONST std::basic_string<T>& wsText, _In_ CONST T* wsParm, _Out_ std::basic_string<T>& wsRetText)
		{
			size_t nIndex = wsText.find(wsParm);
			if (nIndex != -1)
			{
				wsRetText = wsText.substr(0, nIndex);
			}
		}

		template<typename T>
		static void GetRemoveRight(_In_ CONST std::basic_string<T>& wsText, _In_ CONST T* wsParm, _Out_ std::basic_string<T>& wsRetText)
		{
			size_t nIndex = wsText.find(wsParm);
			if (nIndex != -1)
			{
				wsRetText = wsText.substr(nIndex + strlen_my(wsParm));
			}
		}

		template<typename T>
		static UINT GetCount_By_SpecifyText(_In_ CONST std::basic_string<T>& wsText, _In_ CONST T* wsSpecifyText)
		{
			UINT uCount = 0;
			for (std::size_t pos = 0;; ++uCount, ++pos)
			{
				pos = wsText.find(wsSpecifyText, pos);
				if (pos == std::basic_string<T>::npos)
				{
					break;
				}
			}
			return uCount;
		}

		//
		static std::string UnicodeToASCII(_In_ CONST std::wstring& wsText);

		//
		static std::wstring ASCIIToUnicode(_In_ CONST std::string& pszText);

		//
		static std::wstring UTF8ToUnicode(_In_ CONST std::string pszText);

		//
		static std::string UnicodeToUTF8(_In_ CONST std::wstring& wsText);

		enum em_Split_Option
		{
			em_Split_Option_RemoveEmptyEntries = 0x2,
			em_Split_Option_None = 0x4L,
			em_Split_Option_KeepOnly = 0x8L,
			em_Split_Option_KeepOnly_RemoveEmptyEntries = em_Split_Option_RemoveEmptyEntries | em_Split_Option_KeepOnly,
		};
		//
		template<typename T>
		static void Split(_In_ CONST std::basic_string<T>& wsText, _In_ CONST T* wsFormat, _Out_opt_ std::vector<std::basic_string<T>>& vlst, _In_opt_ em_Split_Option emOption)
		{
			std::basic_string<T> wsTmpText = wsText;
			size_t nIndex = wsTmpText.find(wsFormat);
			while (nIndex != -1)
			{
				std::basic_string<T> wstr = wsTmpText.substr(0, nIndex);
				if (emOption & em_Split_Option_RemoveEmptyEntries)
				{
					if (!Trim(wstr).empty())
						vlst.push_back(wstr);
				}
				else if (emOption & em_Split_Option_None)
				{
					vlst.push_back(wstr);
				}

				wsTmpText = wsTmpText.substr(nIndex + strlen_my(wsFormat));
				nIndex = wsTmpText.find(wsFormat);
			}
			if (!wsTmpText.empty() || vlst.size() > 0 || (emOption & em_Split_Option_KeepOnly))
			{
				vlst.push_back(wsTmpText);
			}
		}

		template<typename T>
		static void GetSplit_By_List(_In_ std::basic_string<T>& wsText, _In_ CONST std::vector<std::basic_string<T>>& vSplit, _Out_opt_ std::vector<std::basic_string<T>>& vSplitContent, _In_opt_ em_Split_Option emOption)
		{
			if (vSplit.size() != 2)
				return;

			size_t nLeftIndex = wsText.find(vSplit.at(0));
			size_t nRightIndex = wsText.find(vSplit.at(1));
			if (nLeftIndex == -1 || nRightIndex == -1 || nRightIndex <= nLeftIndex)
				return;

			std::basic_string<T> wsSplit = wsText.substr(nLeftIndex + 1, nRightIndex - nLeftIndex - 1);
			Trim(wsSplit);

			if (emOption | em_Split_Option_RemoveEmptyEntries)
			{
				if (wsSplit != L"")
					vSplitContent.push_back(wsSplit);
			}
			else if (emOption | em_Split_Option_None)
			{
				vSplitContent.push_back(wsSplit);
			}

			wsText = wsText.substr(0, nLeftIndex) + wsText.substr(nRightIndex + 1);
			GetSplit_By_List(wsText, vSplit, vSplitContent, emOption);
		}

		static std::wstring MakeFormatText(_In_ LPCWSTR pwszFormat, ...);

		template<typename T>
		static std::basic_string<T> MakeTextToLower(_In_ CONST std::basic_string<T>& wsText)
		{
			return MakeTextTo(wsText, tolower);
		}

		template<typename T>
		static std::basic_string<T> MakeTextToUpper(_In_ CONST std::basic_string<T>& wsText)
		{
			return MakeTextTo(wsText, toupper);
		}

		template<typename T, typename _fnPtr>
		static std::basic_string<T> MakeTextTo(_In_ CONST std::basic_string<T>& wsText, _In_ _fnPtr fnPtr)
		{
			std::basic_string<T> tmpText;
			for (auto& itm : wsText)
				tmpText.push_back(static_cast<T>(fnPtr(itm)));

			return tmpText;
		}

		// GetVecByParm_RemoveQuotes(L"\"asd\",123,45,\"45\",4",L'\"', Vec={L"asd",L"123",L"34",L"4"})
		static VOID GetVecByParm_RemoveQuotes(_In_ CONST std::wstring& wsText, _In_ WCHAR wchKeyword, _Out_opt_ std::vector<std::wstring>& VecParm);

		// SplitFormatText(L"Map(ShangHai),Point=[51,03]",L"Map(*),Point=[*,*]",Vec={L"ShangHai",L"51",L"03"})
		static BOOL SplitFormatText(_In_ CONST std::wstring& wsText, _In_ CONST std::wstring& wsFormatText, _Out_opt_ std::vector<std::wstring>& VecResult);

		// 
		static std::wstring MakeCurrentPath(_In_ CONST std::wstring& wsText);

		static VOID SetConsoleLanguage();

		static VOID SetSpecialCharacterMode();

		template<typename T>
		static size_t GetHash(_In_ CONST std::basic_string<T>& wsText)
		{
			size_t result = 0;
			for (auto it = str.cbegin(); it != str.cend(); ++it) {
				result = (result * 131) + *it;
			}
			return result;
		}

		// CopyText To Clipboard
		static BOOL CopyTextToClipboard(_In_ LPCWSTR pwchText);										
	private:
		static std::vector<std::wstring> FindSplitFormatText(_In_ CONST std::wstring& wsFormatText);

	};
}


#endif // !__LIBTOOLS_CHARACTERLIB_CHARACTER_H__
