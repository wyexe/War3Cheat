#ifndef __LIBTOOLS_SOCKETCOMMONLIB_SOCKETIOEVENT_H__
#define __LIBTOOLS_SOCKETCOMMONLIB_SOCKETIOEVENT_H__

#include <string>
#include "SocketTag.h"

namespace libTools
{
	class CSocketRemoteClient;
	class CSocketIoEvent : public CSocketTag
	{
	public:
		enum class em_Type
		{
			em_Type_Send,
			em_Type_Recv,
			em_Type_Accept
		};

		enum { SocketIoEvent_Timeout = 60 * 1000 };
	public:
		CSocketIoEvent(_In_ em_Type emType, _In_ CSocketRemoteClient* pSocketClient);
		~CSocketIoEvent() = default;

		//
		OVERLAPPED&                     GetOverlapped();

		//
		em_Type                         GetType() CONST;

		//
		CSocketRemoteClient*			GetSocketClient();

		//
		DWORD							GetSpentTime() CONST;

		//
		std::wstring					GetTypeText() CONST;
	private:
		// 重叠端口 必须放在第一个
		OVERLAPPED                      _Overlapped;
		CSocketRemoteClient*			_pSocketClient;
		em_Type                         _emType;
		ULONGLONG						_ulCreateTick;
	};
}

#endif // !__LIBTOOLS_SOCKETCOMMONLIB_SOCKETIOEVENT_H__
