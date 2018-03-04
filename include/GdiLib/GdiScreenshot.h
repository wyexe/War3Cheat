#ifndef __LIBTOOLS_GDILIB_GDISCREENSHOT_H__
#define __LIBTOOLS_GDILIB_GDISCREENSHOT_H__

#include <Windows.h>
#include <string>
#include <memory>

namespace libTools
{
	class CGdiScreenshot
	{
	public:
		CGdiScreenshot() = default;
		~CGdiScreenshot() = default;

		static BOOL Screenshot(_In_ HWND hWnd, _In_ CONST std::wstring& wsPath);

		static BOOL GetScreenShotContent(_In_ HWND hWnd, _Out_ std::shared_ptr<BYTE>& PicContentPtr, _Out_ UINT& uSize);
	private:

	};
}

#endif // !__LIBTOOLS_GDILIB_GDISCREENSHOT_H__
