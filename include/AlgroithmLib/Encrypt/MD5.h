#ifndef __LIBTOOLS_ALGORITHMLIB_ENCRYPT_MD5_H__
#define __LIBTOOLS_ALGORITHMLIB_ENCRYPT_MD5_H__

#include <Windows.h>
#include <string>

namespace libTools
{
	class CMD5
	{
	private:
		struct MD5_CTX
		{
			UINT Count[2];
			UINT State[4];
			UCHAR Buffer[64];
		};

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x ^ y ^ z)
#define I(x,y,z) (y ^ (x | ~z))
#define MD5_ROTATE(x,n) ((x << n) | (x >> (32-n)))

#define FF(a,b,c,d,x,s,ac) { \
	a += F(b,c,d) + x + ac;  \
	a = MD5_ROTATE(a,s);     \
	a += b;                  \
}

#define GG(a,b,c,d,x,s,ac) { \
	a += G(b,c,d) + x + ac;  \
	a = MD5_ROTATE(a,s);     \
	a += b;                  \
}

#define HH(a,b,c,d,x,s,ac) { \
	a += H(b,c,d) + x + ac;  \
	a = MD5_ROTATE(a,s);     \
	a += b;                  \
}

#define II(a,b,c,d,x,s,ac) { \
	a += I(b,c,d) + x + ac;  \
	a = MD5_ROTATE(a,s);     \
	a += b;                  \
}

	public:
		CMD5();
		~CMD5() = default;

		std::string MD5(_In_ CONST std::string& szPlainText);
	private:
		// initlize
		void InitMd5Ctx();

		//
		void Update(_In_ CONST UCHAR* InBuffer, _In_ UINT uLen);

		//
		void Transform(_In_ CONST UCHAR* Buffer);

		//
		void Decode(_Out_ UCHAR* OutBuffer, _In_ CONST UCHAR* InBuffer, _In_ UINT uLen);

		//
		void Encode(_Out_ UCHAR* OutBuffer, _In_ CONST UCHAR* InBuffer, _In_ UINT uLen);

		//
		VOID Final(_Out_ UCHAR* pszDigest);

		//
		static UINT GetF(_In_ UINT x, _In_ UINT y, _In_ UINT z);
		static UINT GetG(_In_ UINT x, _In_ UINT y, _In_ UINT z);
		static UINT GetH(_In_ UINT x, _In_ UINT y, _In_ UINT z);
		static UINT GetI(_In_ UINT x, _In_ UINT y, _In_ UINT z);
		static UINT Rotate(_In_ UINT x, _In_ UINT n);

		static void SetFF(_In_ _Out_ UINT& a, _In_ UINT b, _In_ UINT c, _In_ UINT d, _In_ UINT x, _In_ UINT s, _In_ UINT ac);
		static void SetGG(_In_ _Out_ UINT& a, _In_ UINT b, _In_ UINT c, _In_ UINT d, _In_ UINT x, _In_ UINT s, _In_ UINT ac);
		static void SetHH(_In_ _Out_ UINT& a, _In_ UINT b, _In_ UINT c, _In_ UINT d, _In_ UINT x, _In_ UINT s, _In_ UINT ac);
		static void SetII(_In_ _Out_ UINT& a, _In_ UINT b, _In_ UINT c, _In_ UINT d, _In_ UINT x, _In_ UINT s, _In_ UINT ac);

		
	private:
		MD5_CTX _Md5Ctx;
	};
}

#endif // !__LIBTOOLS_ALGORITHMLIB_ENCRYPT_MD5_H__
