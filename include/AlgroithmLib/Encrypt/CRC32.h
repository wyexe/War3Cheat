#ifndef __LIBTOOLS_ALGORMTHMLIB_ENCRYP_CRC32_H__
#define __LIBTOOLS_ALGORMTHMLIB_ENCRYP_CRC32_H__

#include <Windows.h>

namespace libTools
{
	class CCRC32
	{
	public:
		CCRC32() = default;
		~CCRC32() = default;

		static DWORD GetCRC32(_In_ LPCSTR pszText, _In_ UINT uSize);
	private:
		
	};
}

#endif // !__LIBTOOLS_ALGORMTHMLIB_ENCRYP_CRC32_H__
