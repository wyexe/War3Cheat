#ifndef __LIBTOOLS_ALGORMTHMLIB_COMMON_COMMONCHARACTER_H__
#define __LIBTOOLS_ALGORMTHMLIB_COMMON_COMMONCHARACTER_H__

#include <Windows.h>

namespace libTools
{
	class CCommonCharacter
	{
	public:
		CCommonCharacter() = default;
		~CCommonCharacter() = default;

		static CHAR ToHex(_In_ UCHAR uc);
	private:

	};
}

#endif // !__LIBTOOLS_ALGORMTHMLIB_COMMON_COMMONCHARACTER_H__
