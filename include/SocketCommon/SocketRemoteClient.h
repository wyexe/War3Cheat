#ifndef __LIBTOOLS_SOCKETCOMMONLIB_SOCKETREMOTECLIENT_H__
#define __LIBTOOLS_SOCKETCOMMONLIB_SOCKETREMOTECLIENT_H__

#include <vector>
#include "SocketTag.h"

namespace libTools
{
	class CSocketBuffer;
	class CSocketRemoteClient : public CSocketTag
	{
	public:
		CSocketRemoteClient(_In_ UINT_PTR Sock);
		virtual ~CSocketRemoteClient() = default;

		//
		VOID    SetTpIo(_In_ TP_IO* pTpIo);

		// 
		TP_IO*  GetTpIo();


		//
		UINT_PTR  GetSocket();

		//      被删除的时候会在线程池中调用该函数, 可以考虑用在处理掉线的时候
		virtual VOID Remove() = NULL;

		//      被添加的时候调用的函数
		virtual VOID Add() = NULL;

		// 是否在线
		virtual BOOL IsOnLine() CONST = NULL;

		// 这个socket是否被使用
		virtual BOOL InExist() CONST;

		// 是否心跳超时
		virtual BOOL IsKeepALiveTimeout() CONST = NULL;

		// 将粘包的缓冲区甩过来
		VOID	SetBuffer(_In_ CONST CHAR* Buffer, _In_ UINT uLen);

		// 清空
		VOID    ClearBuffer();

		// 这个Buffer是当前Socket的收包Buffer
		BYTE*	GetRecvBuffer();

		// 粘包缓冲区的已有长度
		UINT	GetBufferLength() CONST;

		// 转换
		VOID	GetBuffer(_Out_ CSocketBuffer& SocketBuffer);

		// 断开连接
		VOID	DisConnect();

		//
		VOID	Release();

		// 根据Pachet Head来获取这个包的大小
		UINT	GetMaxSize() CONST;

		// 设置Pakcet长度
		VOID    SetMaxSize(_In_ UINT uSize);

		template<typename T>
		T		GetVecBufferValue(_In_ UINT uIndex)
		{
			return uIndex >= _VecBuffer.size() ? T(0) : reinterpret_cast<T*>(_VecBuffer.data())[uIndex];
		}

		UINT	GetRefCount() CONST;

		VOID	AddRefCount();

		VOID	SubRefCount();

		BOOL	ExistPostRecv() CONST;

		VOID	SetExistPostRecv(_In_ BOOL IsExistPostRecv);
	public:
		enum { em_Buffer_Len = 1024 };
	private:
		UINT_PTR           _Socket;
		TP_IO*             _pTpIo;
		std::vector<BYTE>  _VecBuffer;
		UINT			   _uBufferSize;
		UINT			   _uBufferMaxSize;
		BYTE               _RecvBuffer[em_Buffer_Len];
		ULONGLONG		   _ulCreateTick;
		UINT			   _uRefCount;
		BOOL				_ExistPostRecv;
	protected:
		bool			   _bExist;
		bool			   _bLock;
	};
}

#endif // !__LIBTOOLS_SOCKETCOMMONLIB_SOCKETREMOTECLIENT_H__
