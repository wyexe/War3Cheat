#ifndef __LIBTOOLS_HOOKLIB_INLINEHOOK_INLINEHOOK_H__
#define __LIBTOOLS_HOOKLIB_INLINEHOOK_INLINEHOOK_H__

#include <Windows.h>

namespace libTools
{
	class CInlineHook
	{
	public:
		struct HookContent
		{
			DWORD dwHookAddr;				//Hook的地址
			DWORD dwFunAddr;				//自己函数的地址
			UINT  uNopCount;				//要Nop掉的字节数
			DWORD dwOldCode1;				//还原的代码1
			DWORD dwOldCode2;				//还原的代码2
			DWORD dwFlag;					// 保留
			HookContent()
			{
				Release();
			}
			VOID Release()
			{
				dwHookAddr = dwFunAddr = dwOldCode1 = dwOldCode2 = dwFlag = 0;
				uNopCount = 0;
			}
		};
	public:
		CInlineHook() = default;
		~CInlineHook() = default;

		static BOOL Hook(_In_ _Out_ HookContent* pHookContent);

		static BOOL UnHook(_In_ _Out_ HookContent* pHookContent);

		// Hook(需要Hook的函数地址,代替被Hook函数的地址,返回原始函数的入口地址)
		static BOOL Hook(_In_ LPVOID OrgProc, _In_ LPVOID NewProc, _Out_ LPVOID* RealProc);

		// UnHook(需要恢复Hook的函数地址,原始函数的入口地址)
		static BOOL UnHook(_In_ LPVOID OrgProc, _In_ LPVOID RealProc);
	private:

	};
}

#endif // !__LIBTOOLS_HOOKLIB_INLINEHOOK_INLINEHOOK_H__
