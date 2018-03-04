#ifndef __LIBTOOLS_PROCESSLIB_ANTIROOKIT_ANTIROOKIT_H__
#define __LIBTOOLS_PROCESSLIB_ANTIROOKIT_ANTIROOKIT_H__

#include <Windows.h>

namespace libTools
{
	class CAntiRootkit
	{
	public:
		CAntiRootkit() = default;
		~CAntiRootkit() = default;

		//
		BOOL IsAntiLoader();

		//
		BOOL IsExistDebugger();
	private:

	};
}


#endif // !__LIBTOOLS_PROCESSLIB_ANTIROOKIT_ANTIROOKIT_H__
