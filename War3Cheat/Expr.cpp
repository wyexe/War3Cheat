#include "stdafx.h"
#include "Expr.h"
#include <MyTools/Log.h>
#include <MyTools/CLSearchBase.h>
#include <MyTools/Character.h>
#include <MyTools/CLHook.h>
#include <MyTools/CLAsync.h>
#include <MyTools/LdrHeader.h>
#define _SELF L"Expr.cpp"


#define ReadDWORD(x) MyTools::CCharacter::ReadDWORD(x)

VOID CExpr::Release()
{

}

VOID CExpr::Help(CONST std::vector<std::wstring>& Vec)
{
	for (auto& itm : Vec)
	{
		LOG_C_D(L"MethodName=%s", itm.c_str());
	}
}

std::vector<MyTools::ExpressionFunPtr>& CExpr::GetVec()
{
	static std::vector<MyTools::ExpressionFunPtr> Vec = 
	{
		{ std::bind(&CExpr::FindHero,this, std::placeholders::_1) , L"FindHero" },
		{ std::bind(&CExpr::SetHeroPowerType,this, std::placeholders::_1) , L"SetHeroPowerType" },
		{ std::bind(&CExpr::CloseSkillCool,this, std::placeholders::_1) , L"CloseSkillCool" },
		{ std::bind(&CExpr::TestPtr,this, std::placeholders::_1) , L"TestPtr" },
		{ std::bind(&CExpr::Help,this, std::placeholders::_1) , L"Help" },
	};


	return Vec;
}

VOID GetHeroName(_In_ DWORD dwObj, _In_ CHAR* szText)
{
	static DWORD dwGameObjectNameCALL = NULL;
	if (dwGameObjectNameCALL == NULL)
	{
		dwGameObjectNameCALL = MyTools::CLSearchBase::FindCALL("68000200008D44240C", 0x26D7588 - 0x26D7592, (DWORD)::GetModuleHandleW(L"Game.dll"), 1, 0, L"Game.dll");
		if (dwGameObjectNameCALL == NULL)
		{
			LOG_C_E(L"dwGameObjectNameCALL=NULL");
			return;
		}
	}

	__try
	{
		_asm
		{
			PUSHAD;
			PUSH 0x200;
			MOV EAX, szText;
			PUSH EAX;
			MOV ECX, dwObj;
			MOV EAX, dwGameObjectNameCALL;
			CALL EAX;
			POPAD;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		LOG_C_E(L"Exception GetHeroName");
	}
}

static DWORD dwHeroObjectAddr;
BOOL TraverseGameObject(_In_ DWORD dwNodeBase)
{
	if ((ReadDWORD(dwNodeBase + 0xB0) & 3) == 0)
	{
		std::wstring wsName;
		DWORD dwObj = ReadDWORD(dwNodeBase + 0x238);
		if (dwObj != NULL && ReadDWORD(dwObj) != NULL && ReadDWORD(dwNodeBase + 0x248) != NULL && ReadDWORD(dwNodeBase + 0x248 + 0xC) != NULL)
		{
			CHAR szText[MAX_PATH] = { 0 };
			GetHeroName(dwObj, szText);
			if (szText[0] != '\0')
			{
				WCHAR wszText[MAX_PATH];
				MyTools::CCharacter::UTF8ToUnicode(wszText, szText);
				wsName = wszText;
				dwHeroObjectAddr = dwObj;
			}
		}
		LOG_C_D(L"itm.Base=[%X], ObjectAddr=[%X], Name=[%s], HP=[%d/%d], MP=[%d/%d]", 
			dwNodeBase,
			dwObj, 
			wsName.c_str(), 
			ReadDWORD(dwNodeBase + 0x248), 
			ReadDWORD(dwNodeBase + 0x248 + 0x4),
			ReadDWORD(dwNodeBase + 0x248 + 0x8), 
			ReadDWORD(dwNodeBase + 0x248 + 0xC));
	}


	int nNextNodeBase = static_cast<int>(ReadDWORD(dwNodeBase + 0x1C));
	while (nNextNodeBase > 0)
	{
		TraverseGameObject(ReadDWORD(nNextNodeBase + 0xC));
		nNextNodeBase = static_cast<int>(ReadDWORD(nNextNodeBase + 0x8));
	}
	return TRUE;
}

VOID CExpr::FindHero(_In_ CONST std::vector<std::wstring>&)
{
	static DWORD dwGameObjectRoot = NULL;
	if (dwGameObjectRoot == NULL)
	{
		dwGameObjectRoot = MyTools::CLSearchBase::FindBase("EB02DDD88B0D????????3B", 0x267EDBE - 0x267EDC2, 2, 0, L"Game.dll");
		if (dwGameObjectRoot == NULL)
		{
			LOG_C_E(L"dwGameObjectRoot=NULL");
			return;
		}
	}

	TraverseGameObject(ReadDWORD(dwGameObjectRoot));
	LOG_C_D(L"Find Hero = %X", dwHeroObjectAddr);
}

VOID CExpr::SetHeroPowerType(_In_ CONST std::vector<std::wstring>& Vec)
{
	DWORD dwHeroArmorAddr = dwHeroObjectAddr + 0xE4;
	MyTools::CCharacter::WriteDWORD(dwHeroArmorAddr, 0x6);

	DWORD dwHeroAttackAddr = ReadDWORD(dwHeroObjectAddr + 0x1E8) + 0xF4;
	MyTools::CCharacter::WriteDWORD(dwHeroAttackAddr, 0x5);
}

static BOOL bRunSkillChecker = FALSE;
static DWORD dwSkillCheckerParam = 0;
VOID SkillChecker()
{
	DWORD dwObjectAddr = dwSkillCheckerParam + 0x1DC;
	while (bRunSkillChecker)
	{
		::Sleep(100);

		DWORD v9 = 0;
		__asm
		{
			MOV ECX, dwObjectAddr;
			MOV EAX, 0x6F4786B0;
			CALL EAX;
			MOV v9, EAX;
		}
		if (v9 == 0)
		{
			LOG_C_E(L"v9 = 0");
			bRunSkillChecker = FALSE;
			return;
		}
		for (int i = 0; v9 != NULL && (ReadDWORD(v9 + 0x24) & ReadDWORD(v9 + 0x28)) != 0xFFFFFFFF;++i)
		{
			if (ReadDWORD(v9 + 0x20) == 0x210)
			{
				MyTools::CCharacter::WriteDWORD(v9 + 0x20, 0x10);
				MyTools::CCharacter::WriteDWORD(v9 + 0x4, 0x6);
				
				DWORD dwAddr = ReadDWORD(v9 + 0x54);
				if (dwAddr != 0)
				{
					dwAddr = ReadDWORD(dwAddr + 0x54);
					if (dwAddr != 0 && ReadDWORD(dwAddr + 0x10) > 1)
					{
						MyTools::CCharacter::WriteDWORD(dwAddr + 0x10, 1);
					}
				}
			}


			__asm
			{
				PUSHAD;
				MOV ECX, v9;
				ADD ECX, 0x24;
				MOV EAX, 0x6F4786B0;
				CALL EAX;
				MOV v9, EAX;
				POPAD;
			}
		}
	}
	bRunSkillChecker = FALSE;
}

VOID CheckSkill(_In_ DWORD dwSkillObject)
{
	DWORD* pdwSkillObject = new DWORD;
	*pdwSkillObject = dwSkillObject;
	MyTools::CLAsync::GetInstance().ExcuteAsync([pdwSkillObject]
	{
		//*(DWORD*)(*pdwSkillObject + 0x20) = 0x10;
		//*(DWORD*)(*pdwSkillObject + 0x4) = 0x6;
		//LOG_C_D(L"Set Skill Cool");
		DWORD dwAddr = ReadDWORD(*pdwSkillObject + 0x54);
		if (dwAddr != 0)
		{
			DWORD dwCount = ReadDWORD(dwAddr + 0x50);
			DWORD dwSkillArrayAddr = ReadDWORD(dwAddr + 0x54);

			if (dwSkillArrayAddr != 0 && ReadDWORD(dwSkillArrayAddr + 0x10) > 1)
			{
				for (DWORD i = 0;i < dwCount; ++i)
				{
					*(DWORD*)(dwSkillArrayAddr + i * 0x68 + 0x10) = 0x1;
					*(float*)(dwSkillArrayAddr + i * 0x68 + 0x14) = 1.0f;
				}
				
				LOG_C_D(L"Set Skill Mp = 1,dwCount=%d", dwCount);
			}
		}

		delete pdwSkillObject;
	});
	
	//MyTools::CCharacter::WriteDWORD(dwSkillObject + 0x44, 0x1);
}

static DWORD dwHookSkillAddr = 0;
static DWORD s_dwSkillObject = 0;
static DWORD dwHookSkill_EDI = 0;
__declspec(naked) VOID WINAPI HookSkill()
{
	__asm MOV dwHookSkill_EDI, EDI;
	__asm MOV s_dwSkillObject, ESI;
	__asm PUSHAD;

	if (dwHookSkill_EDI == dwHeroObjectAddr)
	{
		CheckSkill(s_dwSkillObject);
	}

	__asm
	{
		POPAD;
		POP EBP;
		ADD ESP, 0xC;
		RETN;
	}
}

VOID CExpr::CloseSkillCool(_In_ CONST std::vector<std::wstring>& Vec)
{
	static MyTools::MYHOOK_CONTENT HookSkillContent;
	HookSkillContent.uNopCount = 0x0;
	HookSkillContent.dwFunAddr = reinterpret_cast<DWORD>(HookSkill);
	dwHookSkillAddr = HookSkillContent.dwHookAddr = MyTools::CLSearchBase::FindAddr("85C075??D9442408D905", 0x6F05C9AD - 0x6F05C9D6, 0, L"Game.dll");
	if (HookSkillContent.dwHookAddr == 0)
	{
		LOG_C_E(L"UnExist HookAddr");
		return;
	}


	MyTools::CLHook::Hook_Fun_Jmp_MyAddr(&HookSkillContent);
	LOG_C_D(L"Hook Succ...");
	/*if (Vec.at(0) == L"0" && HookSkillContent.dwHookAddr != NULL)
	{
		MyTools::CLHook::UnHook_Fun_Jmp_MyAddr(&HookSkillContent);
		HookSkillContent.Release();

		LOG_C_D(L"UnHook Succ...");
	}
	else if (Vec.at(0) == L"1" && HookSkillContent.dwHookAddr == NULL)
	{
		/ *
		6F05C9AD  |.  85C0          test    eax, eax                                       ;  85C075??D9442408D905
		6F05C9AF  |.  75 24         jnz     short 6F05C9D5
		6F05C9B1  |.  D94424 08     fld     dword ptr [esp+8]
		6F05C9B5  |.  D905 70E4AA6F fld     dword ptr [6FAAE470]
		6F05C9BB  |.  DED9          fcompp
		6F05C9BD  |.  DFE0          fstsw   ax
		6F05C9BF  |.  F6C4 05       test    ah, 5
		6F05C9C2  |.  7A 11         jpe     short 6F05C9D5
		6F05C9C4  |.  8B16          mov     edx, dword ptr [esi]
		6F05C9C6  |.  8B92 A4030000 mov     edx, dword ptr [edx+3A4]
		6F05C9CC  |.  8D4424 08     lea     eax, dword ptr [esp+8]
		6F05C9D0  |.  50            push    eax
		6F05C9D1  |.  8BCE          mov     ecx, esi
		6F05C9D3  |.  FFD2          call    edx
		6F05C9D5  |>  5E            pop     esi
		6F05C9D6  |.  5D            pop     ebp                                            ;  Hook
		6F05C9D7  |.  83C4 0C       add     esp, 0C
		6F05C9DA  \.  C3            retn


		* /

		
	}*/
}

BOOL g_PushF2 = FALSE;
BOOL PlayerFlash(_In_ DWORD dwHookPointHeroObjectAddr, _In_ float X, _In_ float Y, _In_ float Z)
{
	if (dwHookPointHeroObjectAddr != dwHeroObjectAddr || !g_PushF2 || (DWORD)X == 0 || (DWORD)Y == 0)
	{
		return FALSE;
	}

	static DWORD dwCALL = MyTools::CLSearchBase::FindCALL("FFD08BC8E83A7F1C00", 0x477C30D - 0x477C311, (DWORD)::GetModuleHandleW(L"Game.dll"), 1, 0, L"Game.dll");
	LOG_C_D(L"Flash [%.2f,%.2f,%.2f]", X, Y, Z);
	struct FlashBufferContent
	{
		float fX;
		float fY;
		float fZ;
	};

	static CHAR Buffer[1024];
	memset(Buffer, 0, sizeof(Buffer));

	((FlashBufferContent*)Buffer)->fX = static_cast<float>(X);
	((FlashBufferContent*)Buffer)->fY = static_cast<float>(Y);
	((FlashBufferContent*)Buffer)->fZ = static_cast<float>(Z);

	__asm
	{
		PUSHAD;
		PUSH 1;
		LEA EAX, Buffer;
		PUSH EAX;
		MOV ECX, dwHeroObjectAddr;
		ADd ECX, 0x164;
		MOV EAX, dwCALL;
		CALL EAX;
		POPAD;
	}
	return TRUE;
}

static DWORD dwHookPointESI = 0;
static DWORD dwHookPointAddr = 0;
static DWORD dwHookPointX = 0;
static DWORD dwHookPointY = 0;
static DWORD dwHookPointZ = 0;
__declspec(naked) void HookPoint()
{
	
	__asm
	{
		MOV dwHookPointESI, ESI;
		LEA EAX, DWORD PTR DS : [ESP + 0x1C8];
		MOV dwHookPointX, EAX;
		LEA EAX, DWORD PTR DS : [ESP + 0x1CC];
		MOV dwHookPointY, EAX;
		LEA EAX, DWORD PTR DS : [ESP + 0x1D0];
		MOV dwHookPointZ, EAX;
	}
	__asm PUSHAD;

	PlayerFlash(ReadDWORD(dwHookPointESI + 0x30),
		MyTools::CCharacter::ReadFloat(dwHookPointX), 
		MyTools::CCharacter::ReadFloat(dwHookPointY),
		MyTools::CCharacter::ReadFloat(dwHookPointZ));

	__asm
	{
		POPAD;
		MOV EDI, dwHookPointAddr; 
		ADD EDI, 0x5;
		PUSH EDI;

		MOV EDI, DWORD PTR DS : [ESI + 0x30];
		TEST EDI, EDI;
		RETN;
	}
}

using PeekMessageAPtr = BOOL(WINAPI*)(_Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax, _In_ UINT wRemoveMsg);
PeekMessageAPtr _OldPeekMessagePtr = nullptr;
BOOL WINAPI PeekMessage_(_Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax, _In_ UINT wRemoveMsg)
{
	BOOL bRetCode = _OldPeekMessagePtr(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
	if (lpMsg->wParam == VK_F2)
	{
		g_PushF2 = lpMsg->message == WM_KEYDOWN ? TRUE : FALSE;
	}
	return bRetCode;
}


VOID CExpr::TestPtr(_In_ CONST std::vector<std::wstring>& Vec)
{
	if (dwHeroObjectAddr == NULL)
	{
		FindHero(Vec);
		if (dwHeroObjectAddr == NULL)
		{
			return;
		}
	}

	SetHeroPowerType(Vec);
	CloseSkillCool(Vec);
	
	static MyTools::MYHOOK_CONTENT HookContent;
	HookContent.uNopCount = 0x0;
	HookContent.dwFunAddr = reinterpret_cast<DWORD>(HookPoint);
	dwHookPointAddr = HookContent.dwHookAddr = MyTools::CLSearchBase::FindAddr("7405E881D3FFFF", 0x46D5B88 - 0x46D5B8F, 0, L"Game.dll");
	if (HookContent.dwHookAddr == 0)
	{
		LOG_C_E(L"UnExist HookAddr");
		return;
	}

	MyTools::CLHook::Hook_Fun_Jmp_MyAddr(&HookContent);

	MyTools::CLdrHeader LdrHeader;
	LdrHeader.InlineHook(::GetProcAddress(::GetModuleHandleW(L"user32.dll"), "PeekMessageA"), PeekMessage_, reinterpret_cast<void **>(&_OldPeekMessagePtr));


	LOG_C_D(L"Hook Point Done! dwHookPointAddr = %X", dwHookPointAddr);
}
