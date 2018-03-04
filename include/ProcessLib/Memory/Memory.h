#ifndef __LIBTOOLS_PROCESSLIB_MEMORY_MEMORY_H__
#define __LIBTOOLS_PROCESSLIB_MEMORY_MEMORY_H__

#include <Windows.h>
#include <string>

namespace libTools
{
	class CMemory
	{
	public:
		CMemory() = default;
		~CMemory() = default;

		//
		static BOOL		ClearWorkingMemory();


		///------------------ Read Memory ...-----------------------------------------------
		template<typename ReturnValueType>
		static ReturnValueType ReadMemoryValue(_In_ UINT_PTR dwAddr)
		{
			ReturnValueType Value = 0;
			::ReadProcessMemory(::GetCurrentProcess(), reinterpret_cast<LPCVOID>(dwAddr), reinterpret_cast<LPVOID>(&Value), sizeof(Value), NULL);
			return Value;
		}

		static UINT_PTR ReadMemValue(_In_ UINT_PTR dwAddr);
		static DWORD	ReadDWORD(_In_ UINT_PTR dwAddr);
		static WORD		ReadWORD(_In_ UINT_PTR dwAddr);
		static BYTE		ReadBYTE(_In_ UINT_PTR dwAddr);
		static FLOAT	ReadFloat(_In_ UINT_PTR dwAddr);
		static double	ReadDouble(_In_ UINT_PTR dwAddr);

		template<typename WriteAddrType, typename WriteValueType>
		static BOOL		WriteMemoryValue(_In_ WriteAddrType dwAddr, _In_ WriteValueType dwValue)
		{
			DWORD dwOldProtect = NULL;
			if (!::VirtualProtect(reinterpret_cast<LPVOID>(dwAddr), sizeof(dwValue), PAGE_EXECUTE_READWRITE, &dwOldProtect))
			{
				return FALSE;
			}

			::WriteProcessMemory(::GetCurrentProcess(), reinterpret_cast<LPVOID>(dwAddr), reinterpret_cast<LPCVOID>(&dwValue), sizeof(dwValue), NULL);
			::VirtualProtect(reinterpret_cast<LPVOID>(dwAddr), sizeof(dwValue), dwOldProtect, &dwOldProtect);
			return TRUE;
		}

		static BOOL		WriteDWORD64(_In_ DWORD64 dwAddr, _In_ DWORD64 dwValue);
		static BOOL		WriteDWORD(_In_ UINT_PTR dwAddr, _In_ DWORD dwValue);
		static BOOL		WriteFloat(_In_ UINT_PTR dwAddr, _In_ FLOAT fValue);
		static BOOL		WriteBYTE(_In_ UINT_PTR dwAddr, _In_ BYTE bValue);


		/// --------Read Memory Text-----------------------------------------------------------------
		static std::wstring ReadUTF8Text(_In_ UINT_PTR dwAddr);
		static std::wstring ReadASCIIText(_In_ UINT_PTR dwAddr);
		static std::wstring ReadUnicodeText(_In_ UINT_PTR dwAddr);
	private:

	};
}

#endif // !__LIBTOOLS_PROCESSLIB_MEMORY_MEMORY_H__
