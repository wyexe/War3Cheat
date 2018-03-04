#ifndef __LIBTOOLS_TIMELIB_TIMETICK_H__
#define __LIBTOOLS_TIMELIB_TIMETICK_H__

#include <Windows.h>

namespace libTools
{
	class CTimeTick
	{
	public:
		enum class em_TimeTick
		{
			em_TimeTick_Hour,
			em_TimeTick_Minute,
			em_TimeTick_Second,
			em_TimeTick_Millisecond
		};
	public:
		CTimeTick();
		CTimeTick(_In_ ULONGLONG ulTick);
		~CTimeTick() = default;

		static ULONGLONG	_GetTickCount64();

		VOID				Reset();

		DWORD				GetSpentTime(_In_ em_TimeTick emTimeTick) CONST;
		DWORD64				GetSpentTime_By_X64(_In_ em_TimeTick emTimeTick) CONST;
	private:
		ULONGLONG _ulTick;
	};
}

#endif // !__LIBTOOLS_TIMELIB_TIMETICK_H__
