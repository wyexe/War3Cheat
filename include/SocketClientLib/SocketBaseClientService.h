#ifndef __LIBTOOLS_SOCKETCLIENTLIB_SOCKETBASECLIENTSERVICE_H__
#define __LIBTOOLS_SOCKETCLIENTLIB_SOCKETBASECLIENTSERVICE_H__

#include <Windows.h>
#include <string>
#include <vector>
#include <mutex>

namespace libTools
{
	class CSocketIoEvent;
	class CSocketBuffer;

	class CSocketBaseClientService
	{
	public:
		enum { em_Buffer_Length = 1024 };

		struct SocketSendBuffer
		{
			CHAR* Buffer = nullptr;
			UINT	uMaxLength = 0;
			UINT	uCurLength = 0;
		};
	public:
		CSocketBaseClientService();
		~CSocketBaseClientService();

		BOOL Run(_In_ CONST std::wstring& wsIp, _In_ SHORT Port, _In_ DWORD dwMaxTimeout);

		VOID Stop();

		BOOL PostSend(_In_ CSocketBuffer& SocketBuffer);

		bool IsConnect() CONST;
	private:
		//////static ////////////////////////////////////////////////////////////////////

		// 完成端口回调
		static VOID CALLBACK IoCompletionCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PVOID Overlapped, ULONG IoResult, ULONG_PTR NumberOfBytesTransferred, PTP_IO Io);

		BOOL PostSend(_In_ SocketSendBuffer* pSendSocket);
	protected:
		BOOL PostConnect();

		VOID OnConnect();

		VOID OnClose();

		VOID OnRecv(_In_ DWORD NumberOfBytesTransferred);

		VOID OnSend(_In_ CSocketIoEvent* pIoEvent, _In_ UINT NumberOfBytesTransferred);

		VOID PostRecv();

		virtual VOID EchoPacket(_In_ CSocketBuffer& SocketBuffer) = NULL;

		virtual VOID DisConnectServer() = NULL;

		VOID Clear();
	private:
		/////Network/////////////////////////////////////////////////////////////////////

		//
		BOOL CreateServerSocket();

		BOOL ConnectEx(_In_ sockaddr* pSockaddr, _In_ std::size_t uAddrLen, _Out_ LPOVERLAPPED lpOverlapped) CONST;

		BOOL WaitToConnect(_In_ DWORD dwMaxTimeOut);
	private:
		//////Buffer////////////////////////////////////////////////////////////////////
		VOID SetBuffer(_In_ CONST CHAR* Buffer, _In_ DWORD NumberOfBytesTransferred);
	protected:
		bool				_bConnect;
	private:
		UINT_PTR			_ServerSocket;

		TP_IO*				_pServerTpIo;

		DWORD				_dwRecvSize;

		CHAR				_RecvBuffer[em_Buffer_Length];

		std::vector<CHAR>	_VecBuffer;

		std::mutex			_MtxRecv;

		DWORD				_dwMaxTimeout;

		std::wstring		_wsIp;

		SHORT				_Port;

		BOOL				_ExistPostRecv;
	};
}

#endif // !__LIBTOOLS_SOCKETCLIENTLIB_SOCKETBASECLIENTSERVICE_H__
