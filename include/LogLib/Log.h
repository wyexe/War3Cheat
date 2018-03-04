#ifndef __LIBTOOLS_LOGLIB_LOG_H__
#define __LIBTOOLS_LOGLIB_LOG_H__

#include <Windows.h>
#include <string>
#include <queue>
#include <mutex>

#define LOG_C(Type,FormatText,...)	libTools::CLog::GetInstance().Print(__FUNCTIONW__, _SELF, __LINE__, libTools::CLog::em_Log_OutputType_Console, Type, FALSE, FormatText, __VA_ARGS__)
#define LOG_F(Type,FormatText,...)	libTools::CLog::GetInstance().Print(__FUNCTIONW__, _SELF, __LINE__, libTools::CLog::em_Log_OutputType_File, Type, FALSE, FormatText, __VA_ARGS__)
#define LOG_CF(Type,FormatText,...) libTools::CLog::GetInstance().Print(__FUNCTIONW__, _SELF, __LINE__, libTools::CLog::em_Log_OutputType_Console | libTools::CLog::em_Log_OutputType_File, Type, FALSE, FormatText, __VA_ARGS__)
#define LOG_FC(Type,FormatText,...) LOG_CF(Type, FormatText, __VA_ARGS__)
#define LOG_MSG_CF(FormatText,...)  libTools::CLog::GetInstance().Print(__FUNCTIONW__, _SELF, __LINE__, libTools::CLog::em_Log_OutputType_Console | libTools::CLog::em_Log_OutputType_File, libTools::CLog::em_Log_Type::em_Log_Type_Exception, TRUE, FormatText, __VA_ARGS__)
#define LOG_CF_D(FormatText,...)	LOG_CF(libTools::CLog::em_Log_Type::em_Log_Type_Debug,FormatText, __VA_ARGS__)
#define LOG_CF_E(FormatText,...)	LOG_CF(libTools::CLog::em_Log_Type::em_Log_Type_Exception,FormatText, __VA_ARGS__)
#define LOG_C_D(FormatText,...)		LOG_C(libTools::CLog::em_Log_Type::em_Log_Type_Debug,FormatText, __VA_ARGS__)
#define LOG_C_E(FormatText,...)		LOG_C(libTools::CLog::em_Log_Type::em_Log_Type_Exception,FormatText, __VA_ARGS__)
#define CON_LOG_CF_D(Con,FormatText,...)  if(Con) { LOG_CF_D(FormatText, __VA_ARGS__); }
#define CON_LOG_CF_E(Con,FormatText,...)  if(Con) { LOG_CF_E(FormatText, __VA_ARGS__); }

namespace libTools
{
	class CLog
	{
	public:
		enum class em_Log_Type
		{
			em_Log_Type_Invalid,
			em_Log_Type_Debug,
			em_Log_Type_Warning,
			em_Log_Type_Custome,
			em_Log_Type_Exception,
		};

		enum em_Log_OutputType
		{
			em_Log_OutputType_File = 0x2,
			em_Log_OutputType_Console = 0x4
		};

	private:
		enum { MAX_SAVELOG_COUNT = 1024 * 2 };

#pragma pack(push)
#pragma pack(4)
		struct LogContent
		{
			em_Log_Type emLogType;

			// FunctionName
			std::wstring wsFunName;

			// FileName
			std::wstring wsFileName;

			// Log Line in Code
			UINT uLine;

			// ÄÚÈÝ
			std::wstring wsContent;

			SYSTEMTIME SysTime;
		};
#pragma pack(pop)
	public:
		CLog();
		~CLog();

		static CLog& GetInstance();

	public:
		VOID Print(_In_ LPCWSTR pwszFunName, _In_ LPCWSTR pwszFileName, _In_ int nLine, _In_ int nLogOutputType, _In_ em_Log_Type emLogType, _In_ BOOL bMsgBox, _In_ LPCWSTR pwszFormat, ...);

		// SaveLogPath have to like 'C:\\'
		VOID SetClientName(_In_ CONST std::wstring& cwsClientName, _In_ CONST std::wstring wsSaveLogPath);

		// Mutiple Thread UnSafe, be care use!
		VOID SetClientName(_In_ CONST std::wstring& cwsClientName);

		VOID Release();

		VOID SaveLog_Immediately(_In_ BOOL bAppend);

		VOID ClearLogFile();

		VOID SetSyncSendLog() _NOEXCEPT;

		VOID SetInfiniteSave() _NOEXCEPT;

		UINT FormLogSize();

		UINT SaveLogSize();

		VOID Clear();
	private:
		VOID SaveLog(_In_ CONST std::wstring& wsContent, _In_ BOOL bAppend);

		static DWORD WINAPI _SendThread(LPVOID lpParm);
		static DWORD WINAPI _SaveThread(LPVOID lpParm);

		VOID AddLogContentToQueue(_In_ CONST LogContent& LogContent_);

		BOOL GetLogContentForQueue(_Out_ LogContent& LogContent_);

		VOID ClearLogConttent();

		VOID AddSaveLogToQueue(_In_ CONST LogContent& LogContent_);

		VOID GetSaveLogContentForQueue(_Out_ std::deque<LogContent>& VecLogContent);

		std::wstring FormatLogContent(_In_ CONST LogContent& Content) CONST;

		BOOL IsSaveLog();

		VOID ClearSaveLog();

		VOID SendLogToForm(_In_ CONST LogContent& Content) CONST;
	private:
		struct LogFormContent
		{
			DWORD	dwPid = NULL;
			HWND	hWnd = NULL;
		};
	private:
		BOOL   ExistLogServer();

		static BOOL CALLBACK EnumSetWinName(HWND hWnd, LPARAM lpParam);
	private:
		std::deque<LogContent> _QueueLogContent;
		std::deque<LogContent> _QueueSaveLogContent;
		bool				   _bRun;
		std::wstring           _wsLogPath;
		std::wstring           _wsClientName;
		HANDLE                 hSaveLogEvent;
		HANDLE                 hSendExitEvent;
		std::mutex             _MtxFormLogContent;
		std::mutex             _MtxSaveLogContent;
		LogFormContent         _LogFormContent;
		bool                   _bSync;
		bool				   _InfiniteSave;
	private:
		CLog& operator = (CONST CLog&) = delete;
		CLog(CONST CLog&) = delete;
	};
}

#endif // !__LIBTOOLS_LOGLIB_LOG_H__
