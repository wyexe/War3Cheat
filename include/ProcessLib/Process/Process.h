#ifndef __LIBTOOLS_PROCESSLIB_PROCESS_PROCESS_H__
#define __LIBTOOLS_PROCESSLIB_PROCESS_PROCESS_H__

#include <Windows.h>
#include <string>
#include <vector>

namespace libTools
{
	class CProcess
	{
	public:
		struct ProcessContent
		{
			DWORD dwPid;
			std::wstring wsClientName; // make lower...
		};
	public:
		CProcess() = default;
		~CProcess() = default;

		// Find Proc ...
		static DWORD FindPidByProcName(_In_ CONST std::wstring& wsProcName);

		// Find Proc ...
		static BOOL  IsExistPid(_In_ DWORD dwPid);

		// Find Proc ...
		static BOOL  IsExistProcName(_In_ CONST std::wstring& wsProcName);

		// Terminate Process ...
		static VOID  TerminateProcByProcName(_In_ CONST std::wstring& wsProcName);

		// 
		static VOID  TerminateProcByPid(_In_ DWORD dwPid);

		// Get Vec Process(from Snapshot process)
		static BOOL  GetVecProcess(_Out_ std::vector<ProcessContent>& VecProc);

		//
		static int	 GetCpuUsageByPid(_In_ DWORD dwPid, _In_ _Out_ LONGLONG& llLastTime, _In_ _Out_ LONGLONG& llLastSysTime);

		// Memory usage...
		static ULONGLONG CalcWorkSetPrivate(_In_ DWORD dwPid);

		//
		static BOOL CreateProc(_In_ CONST std::wstring& wsProcPath, _In_ BOOL IsSupended, _Out_opt_ PROCESS_INFORMATION* pRetProcInfo = nullptr);
	private:

	};
}

#endif // !__LIBTOOLS_PROCESSLIB_PROCESS_PROCESS_H__
