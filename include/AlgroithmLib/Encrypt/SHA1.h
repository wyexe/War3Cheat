#ifndef __LIBTOOLS_ALGORMTHMLIB_ENCRYP_SHA1_H__
#define __LIBTOOLS_ALGORMTHMLIB_ENCRYP_SHA1_H__

#include <Windows.h>
#include <string>

namespace libTools
{
	class CSHA1
	{
	private:
		struct SHA_CTX 
		{
			UINT State[5];
			UINT Count[2];
			UINT Buffer[80];
			INT  LenBuffer;
		};
	public:
		CSHA1();
		~CSHA1() = default;

		std::string GetSHA1(_In_ CONST std::string& szPlainText);
	private:
		//
		void Transform();

		//
		void InitSHA1Ctx();

		//
		void Update(_In_ CONST UCHAR* InBuffer, UINT uLen);

		//
		void Final(_Out_ UCHAR* pszDigest);
	private:
		SHA_CTX _ShaCtx;
	};
}

#endif // !__LIBTOOLS_ALGORMTHMLIB_ENCRYP_SHA1_H__
