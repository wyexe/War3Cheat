#ifndef __LIBTOOLS_PROCESSLIB_KEYBOARDMSG_KEYBOARDMSG_H__
#define __LIBTOOLS_PROCESSLIB_KEYBOARDMSG_KEYBOARDMSG_H__

#include <Windows.h>

namespace libTools
{
	class CKeyboardMsg
	{
	public:
		enum em_Custome_Key
		{
			VK_0			= 48,
			VK_1			= 49,
			VK_2			= 50,
			VK_3			= 51,
			VK_4			= 52,
			VK_5			= 53,
			VK_6			= 54,
			VK_7			= 55,
			VK_8			= 56,
			VK_9			= 57,
			VK_D			= 59,
			VK_J			= 58,
			VK_ARROW_TOP	= 38,
			VK_ARROW_LEFT	= 37,
			VK_ARROW_RIGHT	= 39,
			VK_ARROW_DOWN	= 40,
			VK_SHIFT_0		= 0x101,
			VK_SHIFT_1		= 0x102,
			VK_SHIFT_2		= 0x103,
			VK_SHIFT_3		= 0x104,
			VK_SHIFT_4		= 0x105,
			VK_SHIFT_5		= 0x106,
			VK_SHIFT_6		= 0x107,
			VK_SHIFT_7		= 0x108,
			VK_SHIFT_8		= 0x109,
			VK_SHIFT_9		= 0x10A,
			VK_SHIFT_D		= 0x10B,
			VK_SHIFT_J		= 0x10C,
		};
	public:
		CKeyboardMsg() = default;
		~CKeyboardMsg() = default;

		// 模拟按键
		static BOOL		SimulationKey(_In_  HWND hWnd, _In_ CHAR dwASCII, _In_opt_ DWORD dwTime = 10);

		// 获取虚拟码
		static DWORD	GetVirKey(_In_ CHAR dwASCII);

		// 模拟鼠标
		static BOOL		SimulationMouse(_In_ int x, _In_ int y);

		// 模拟鼠标
		static BOOL		SimulationRightMouse(_In_ int x, _In_ int y);

		// 发送文本
		static BOOL		SendAscii(_In_ WCHAR data, _In_  BOOL shift);

		// 发送文本
		static BOOL		SendUnicode(_In_ WCHAR data);

		// 模拟按键
		static BOOL		SendKey(_In_ WORD wVk);

		// 发送文本
		static BOOL		SendKeys(_In_ LPCWSTR data);

		// 发送文本
		static BOOL		SendKeys(_In_ LPCSTR data);
	private:

	};
}

#endif // !__LIBTOOLS_PROCESSLIB_KEYBOARDMSG_KEYBOARDMSG_H__
