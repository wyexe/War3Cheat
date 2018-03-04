#ifndef __LIBTOOLS_HOOKLIB_IATHOOK_IATHOOK_H__
#define __LIBTOOLS_HOOKLIB_IATHOOK_IATHOOK_H__

#include <Windows.h>
#include <string>
namespace libTools
{
	// Compatible x86 & x64
	class CIATHook
	{
	public:
		CIATHook() = default;
		~CIATHook() = default;

		static BOOL Hook(_In_ CONST std::string& szDLLName, _In_ CONST std::string& szMethodName, _In_ LPVOID HookProcPtr, _Out_opt_ LPVOID* pRealProcPtr);
	private:

	};
}

#endif // !__LIBTOOLS_HOOKLIB_IATHOOK_IATHOOK_H__
