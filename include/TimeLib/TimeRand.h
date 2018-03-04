#ifndef __LIBTOOLS_TIMELIB_TIMERAND_H__
#define __LIBTOOLS_TIMELIB_TIMERAND_H__

#include <Windows.h>

namespace libTools
{
	class CTimeRand
	{
	public:
		CTimeRand() = default;
		~CTimeRand() = default;

		static int GetRand(_In_ int nMinValue, _In_ int nMaxValue);
	private:

	};
}

#endif // !__LIBTOOLS_TIMELIB_TIMERAND_H__
