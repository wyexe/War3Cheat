#ifndef __LIBTOOLS_LOGLIB_CMDLOG_H__
#define __LIBTOOLS_LOGLIB_CMDLOG_H__

#include <string>
#include "LogExpression.h"

namespace libTools
{
	class CCmdLog
	{
	public:
		enum
		{
			em_CmdLog_Port = 12344,
			em_CmdLog_Msg_SendClientInfo = 0x1,
			em_CmdLog_MaxClientCount = 100
		};

		struct SendText
		{
			DWORD dwParm1;
			DWORD dwParm2;
			DWORD dwMsg;
			WCHAR wszText[1024];
		};

		struct RecvText
		{
			DWORD dwParm1;
			DWORD dwParm2;
			WCHAR wszText[1024];
		};
	public:
		CCmdLog();
		~CCmdLog();

		BOOL Run(_In_ CONST std::wstring& wsClientName, _In_ CONST std::vector<ExpressionFunPtr>& ParmVecFunPtr);

		VOID Stop();

		VOID ReSetClientName(_In_ CONST std::wstring& wsClientName);

		VOID ExcuteLogServerCmd(_In_ CONST std::wstring& wsCmdText);

		VOID EnableGloablMethod();
	private:
		static DWORD WINAPI _RecvThread(LPVOID lpParm);



		BOOL ConnectLogServer();

		VOID BreakLogConnect();

		BOOL SendClientName();

		int Send(_In_ CONST CHAR* Buffer, _In_ INT nLen);

		BOOL Recv(_Out_ std::wstring& wsCmdText);
	private:
		CCmdLog(CONST CCmdLog&) = delete;
		CCmdLog(CONST CCmdLog&&) = delete;
		CCmdLog& operator= (CONST CCmdLog&) = delete;
	private:
		UINT_PTR		_skClient;
		std::wstring	_wsClientName;
		BOOL			_Run;
		HANDLE			_hRecvThread;
		CLogExpression	_LogExpression;
	};
}




#endif // !__LIBTOOLS_LOGLIB_CMDLOG_H__
