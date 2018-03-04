#ifndef __LIBTOOLS_EXCEPTIONLIB_EXCEPTION_H__
#define __LIBTOOLS_EXCEPTIONLIB_EXCEPTION_H__

#include <Windows.h>
#include <functional>
#include <string>

namespace libTools
{
	extern std::function<VOID(CONST std::wstring&)> g_EchoExceptionMsgPtr;

	class CException
	{
	public:
		CException() = default;
		~CException() = default;

		static VOID InvokeAction(_In_ LPCWSTR pwszFunName, _In_ CONST std::function<VOID(VOID)>& MethodPtr)
		{
			__try
			{
				MethodPtr();
			}
			__except (PrintExceptionCode(GetExceptionInformation()))
			{
				InvokeExceptionPtr(L"InvokeAction Exception[%s]", pwszFunName);
			}
		}

		template<typename T>
		static T InvokeFunc(_In_ LPCWSTR pwszFunName, _In_ CONST std::function<T(VOID)>& MethodPtr)
		{
			__try
			{
				return MethodPtr();
			}
			__except (PrintExceptionCode(GetExceptionInformation()))
			{
				InvokeExceptionPtr(L"InvokeFunc Exception[%s]", pwszFunName);
			}
			return T();
		}

	private:
		static VOID InvokeExceptionPtr(_In_ LPCWSTR pwszFormat, ...);

		static BOOL PrintExceptionCode(_In_ LPEXCEPTION_POINTERS ExceptionPtr);
	};

}
#endif // !__LIBTOOLS_EXCEPTIONLIB_EXCEPTION_H__
