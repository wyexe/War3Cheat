#ifndef __LIBTOOLS_SOCKETCOMMONLIB_SOCKETTAG_H__
#define __LIBTOOLS_SOCKETCOMMONLIB_SOCKETTAG_H__

#include <Windows.h>

namespace libTools
{
	class CSocketTag
	{
	public:
		CSocketTag() = default;
		~CSocketTag() = default;

		template<typename T>
		void SetTag(T* Ptr)
		{
			_lpAddr = reinterpret_cast<LPVOID>(Ptr);
		}

		template<typename T>
		T* GetTag()
		{
			return reinterpret_cast<T*>(_lpAddr);
		}
	private:
		LPVOID _lpAddr = nullptr;
	};
}

#endif // !__LIBTOOLS_SOCKETCOMMONLIB_SOCKETTAG_H__
