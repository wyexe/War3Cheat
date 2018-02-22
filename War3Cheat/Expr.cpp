#include "stdafx.h"
#include "Expr.h"
#include <MyTools/Log.h>
#include <MyTools/CLSearchBase.h>
#include <MyTools/Character.h>
#include <MyTools/CLHook.h>
#include <MyTools/CLAsync.h>
#include <MyTools/LdrHeader.h>
#include <MyTools/CLEchoException.h>
#include "FindGameObject.h"
#define _SELF L"Expr.cpp"


#define ReadDWORD(x) MyTools::CCharacter::ReadDWORD(x)

VOID CExpr::Release()
{

}

VOID CExpr::Help(CONST std::vector<std::wstring>& Vec)
{
	for (auto& itm : GetVec())
	{
		LOG_C_D(L"MethodName=%s", itm.wsFunName.c_str());
	}
}

std::vector<MyTools::ExpressionFunPtr>& CExpr::GetVec()
{
	static std::vector<MyTools::ExpressionFunPtr> Vec = 
	{
		{ std::bind(&CExpr::PrintSelectedObject,this, std::placeholders::_1) , L"PrintSelectedObject" },
		{ std::bind(&CExpr::SetSelectedIsHero,this, std::placeholders::_1) , L"SetSelectedIsHero" },
		{ std::bind(&CExpr::SetSelectedObjectPowerType,this, std::placeholders::_1) , L"SetSelectedObjectPowerType" },
		{ std::bind(&CExpr::SetSelectedObjectInvincible,this, std::placeholders::_1) , L"SetSelectedObjectInvincible" },
		{ std::bind(&CExpr::CloseSkillCool,this, std::placeholders::_1) , L"CloseSkillCool" },
		{ std::bind(&CExpr::TestPtr,this, std::placeholders::_1) , L"TestPtr" },
		{ std::bind(&CExpr::PrintItem,this, std::placeholders::_1) , L"PrintItem" },
		{ std::bind(&CExpr::ChangeItem,this, std::placeholders::_1) , L"ChangeItem" },
		{ std::bind(&CExpr::Help,this, std::placeholders::_1) , L"Help" },
	};


	return Vec;
}

static DWORD dwHeroObjectAddr;

VOID CExpr::SetSelectedIsHero(_In_ CONST std::vector<std::wstring>&)
{
	CFindGameObject FindGameObject;
	DWORD dwGameNodeBase = FindGameObject.FindSelectedObject();
	dwHeroObjectAddr = FindGameObject.GetGameObjectAddr(dwGameNodeBase);
	LOG_C_D(L"Set Hero[%s].dwNodeBase=%X, ObjAddr=%X", FindGameObject.GetSelectedObjectName().c_str(), dwGameNodeBase, dwHeroObjectAddr);
}

VOID CExpr::PrintSelectedObject(_In_ CONST std::vector<std::wstring>& Vec)
{
	CFindGameObject FindGameObject;
	DWORD dwGameNodeBase = FindGameObject.FindSelectedObject();
	LOG_C_D(L"Selected[%s].dwNodeBase=%X, ObjAddr=%X", FindGameObject.GetSelectedObjectName().c_str(), dwGameNodeBase, FindGameObject.GetGameObjectAddr(dwGameNodeBase));
}

VOID CExpr::SetSelectedObjectPowerType(_In_ CONST std::vector<std::wstring>& Vec)
{
	CFindGameObject FindGameObject;
	DWORD dwGameNodeBase = FindGameObject.GetGameObjectAddr(FindGameObject.FindSelectedObject());

	DWORD dwHeroArmorAddr = dwGameNodeBase + 0xE4;
	MyTools::CCharacter::WriteDWORD(dwHeroArmorAddr, 0x6);

	DWORD dwHeroAttackAddr = ReadDWORD(dwGameNodeBase + 0x1E8) + 0xF4;
	MyTools::CCharacter::WriteDWORD(dwHeroAttackAddr, 0x5);
}

VOID CExpr::SetSelectedObjectInvincible(_In_ CONST std::vector<std::wstring>& Vec)
{
	CFindGameObject FindGameObject;
	DWORD dwGameNodeBase = FindGameObject.GetGameObjectAddr(FindGameObject.FindSelectedObject());

	MyTools::CCharacter::WriteBYTE(dwGameNodeBase + 0x20, 0xE);
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

			if (dwSkillArrayAddr != 0 && (MyTools::CCharacter::ReadFloat(dwSkillArrayAddr + 0x14) > 1.0f || ReadDWORD(dwSkillArrayAddr + 0x10) > 1))
			{
				for (DWORD i = 0; i < dwCount; ++i)
				{
					*(DWORD*)(dwSkillArrayAddr + i * 0x68 + 0x10) = 0x0;
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
	/*
		6F05C9AD | .  85C0          test    eax, eax;                   ; 85C075??D9442408D905
		6F05C9AF | .  75 24         jnz     short 6F05C9D5
		6F05C9B1 | .D94424 08     fld     dword ptr[esp + 8]
		6F05C9B5 | .D905 70E4AA6F fld     dword ptr[6FAAE470]
		6F05C9BB | .DED9          fcompp
		6F05C9BD | .DFE0          fstsw   ax
		6F05C9BF | .F6C4 05       test    ah, 5
		6F05C9C2 | .  7A 11         jpe     short 6F05C9D5
		6F05C9C4 | .  8B16          mov     edx, dword ptr[esi]
		6F05C9C6 | .  8B92 A4030000 mov     edx, dword ptr[edx + 3A4]
		6F05C9CC | .  8D4424 08     lea     eax, dword ptr[esp + 8]
		6F05C9D0 | .  50            push    eax
		6F05C9D1 | .  8BCE          mov     ecx, esi
		6F05C9D3 | .FFD2          call    edx
		6F05C9D5 | > 5E            pop     esi
		6F05C9D6 | .  5D            pop     ebp;						;Hook
		6F05C9D7 | .  83C4 0C       add     esp, 0C
		6F05C9DA  \.C3            retn
	*/

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
VOID CExpr::HookPointFlash(_In_ CONST std::vector<std::wstring>&)
{
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



/*
6F32D3D8  |.  50            push    eax
6F32D3D9  |.  57            push    edi
6F32D3DA  |.  B9 2CC7AC6F   mov     ecx, 6FACC72C							   ;  ResNamePtr
6F32D3DF  |.  E8 6CCFFEFF   call    6F31A350                                   ;  GetNameFlag2 ...
6F32D3E4  |.  85C0          test    eax, eax

--------------------------------------
6F31A350  /$  8B41 24       mov     eax, dword ptr [ecx+24]                    ;  Flag2 = 08DB8AAE
6F31A353  |.  83F8 FF       cmp     eax, -1
6F31A356  |.  75 05         jnz     short 6F31A35D
6F31A358  |.  33C0          xor     eax, eax
6F31A35A  |.  C2 0800       retn    8
6F31A35D  |>  8B51 1C       mov     edx, dword ptr [ecx+1C]
6F31A360  |.  53            push    ebx
6F31A361  |.  56            push    esi
6F31A362  |.  8B7424 0C     mov     esi, dword ptr [esp+C]
6F31A366  |.  23C6          and     eax, esi                                   ;  3FF
6F31A368  |.  8D0440        lea     eax, dword ptr [eax+eax*2]                 ;  *3
6F31A36B  |.  8D4482 04     lea     eax, dword ptr [edx+eax*4+4]
6F31A36F  |.  8B40 04       mov     eax, dword ptr [eax+4]
6F31A372  |.  33D2          xor     edx, edx
6F31A374  |.  85C0          test    eax, eax
6F31A376  |.  0F9EC2        setle   dl
6F31A379  |.  57            push    edi
6F31A37A  |.  83EA 01       sub     edx, 1
6F31A37D  |.  23C2          and     eax, edx
6F31A37F  |.  85C0          test    eax, eax
6F31A381  |.  7E 29         jle     short 6F31A3AC
6F31A383  |.  8B7C24 14     mov     edi, dword ptr [esp+14]
6F31A387  |>  3970 04       /cmp     dword ptr [eax+4], esi
6F31A38A  |.  75 07         |jnz     short 6F31A393
6F31A38C  |.  8B50 18       |mov     edx, dword ptr [eax+18]                   ;  dd [[6FACC72C+1C]+2030]+18 = NameFlag
6F31A38F  |.  3B17          |cmp     edx, dword ptr [edi]
6F31A391  |.  74 1B         |je      short 6F31A3AE
6F31A393  |>  8B59 1C       |mov     ebx, dword ptr [ecx+1C]
6F31A396  |.  8BD6          |mov     edx, esi
6F31A398  |.  2351 24       |and     edx, dword ptr [ecx+24]
6F31A39B  |.  8D1452        |lea     edx, dword ptr [edx+edx*2]
6F31A39E  |.  8D1493        |lea     edx, dword ptr [ebx+edx*4]
6F31A3A1  |.  8B12          |mov     edx, dword ptr [edx]
6F31A3A3  |.  03D0          |add     edx, eax
6F31A3A5  |.  8B42 04       |mov     eax, dword ptr [edx+4]
6F31A3A8  |.  85C0          |test    eax, eax
6F31A3AA  |.^ 7F DB         \jg      short 6F31A387
6F31A3AC  |>  33C0          xor     eax, eax
6F31A3AE  |>  5F            pop     edi
6F31A3AF  |.  5E            pop     esi
6F31A3B0  |.  5B            pop     ebx
6F31A3B1  \.  C2 0800       retn    8
*/

DWORD GetNamePtr(_In_ DWORD dwNameFlag, _In_ DWORD dwCALLAddr)
{
	__try
	{
		DWORD dwRetCode = 0;
		
		__asm
		{
			PUSHAD;

			XOR EDX, EDX;
			MOV ECX, dwNameFlag;
			MOV EAX, dwCALLAddr;
			CALL EAX;
			MOV dwRetCode, EAX;
			POPAD;
		}

		return dwRetCode;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		__asm PUSHAD;
		LOG_C_E(L"GetNamePtr Exception, dwNameFlag=%X", dwNameFlag);
		__asm POPAD;
	}
	return 0;
}

DWORD GetItemDetail(_In_ DWORD dwNameFlag, _In_ DWORD dwCALLAddr)
{
	__try
	{
		DWORD dwRetCode = 0;

		__asm
		{
			PUSHAD;

			PUSH 1;
			XOR EDX, EDX;
			MOV ECX, dwNameFlag;
			MOV EAX, dwCALLAddr;
			CALL EAX;
			MOV dwRetCode, EAX;
			POPAD;
		}

		return dwRetCode;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		__asm PUSHAD;
		LOG_C_E(L"GetItemDetail Exception, dwNameFlag=%X", dwNameFlag);
		__asm POPAD;
	}
	return 0;
}

VOID CExpr::PrintItem(_In_ CONST std::vector<std::wstring>& Vec)
{
	DWORD dwCALLAddr = MyTools::CLSearchBase::FindCALL("E8????????33D28BCBE8????????837C242000", 0x6F36A826 - 0x6F36A82F, (DWORD)(::GetModuleHandleW(L"Game.dll")), 1, 0, L"Game.dll");
	if (dwCALLAddr == 0)
	{
		LOG_C_E(L"dwCALLAddr = 0 ");
		return;
	}

	DWORD dwItemDetailCALLAddr = MyTools::CLSearchBase::FindCALL("6A0133D2E8????????8BD8EB", 0x6F36A841 - 0x6F36A845, (DWORD)(::GetModuleHandleW(L"Game.dll")), 1, 0, L"Game.dll");
	if (dwItemDetailCALLAddr == 0)
	{
		LOG_C_E(L"dwItemDetailCALLAddr = 0 ");
		return;
	}

	DWORD dwResNamePtr = MyTools::CLSearchBase::FindBase("B9????????E8????????85C07526", 0x6F32D3DA - 0x6F32D3DA, 1, 0, L"Game.dll", 0xFFFFFFFF);
	LOG_C_D(L"dwResNamePtr = %X", dwResNamePtr);
	if (dwResNamePtr == 0)
		return;

	MyTools::CLEchoException::GetInstance().InvokeAction(__FUNCTIONW__, [=]
	{
		DWORD dwNameFlagAddr = ReadDWORD(dwResNamePtr + 0x1C);
		DWORD dwCount = ReadDWORD(dwResNamePtr + 0x24);
		LOG_C_D(L"dwCount=%d", dwCount);
		for (DWORD i = 2; i < dwCount; ++i)
		{
			DWORD dwNameFlagObjAddr = ReadDWORD(dwNameFlagAddr + 0xC * i + 0x8);
			if (dwNameFlagObjAddr == 0)
				continue;

			DWORD dwNameFlag = ReadDWORD(dwNameFlagObjAddr + 0x18);
			DWORD dwFlag2 = ReadDWORD(dwNameFlagObjAddr + 0x4);
			if (dwNameFlag == 0 || dwFlag2 == 0)
				continue;

			if ((dwNameFlag >> 0x18) != 'I')
				continue;

			CHAR szText[32] = { 0 };
			//dwNameFlag = (dwNameFlag & 0xFF) << 0x18 | ((dwNameFlag >> 0x8 & 0xFF) << 0x10) | ((dwNameFlag >> 0x10 & 0xFF) << 0x8) | (dwNameFlag >> 0x18 & 0xFF);
			MyTools::CCharacter::strcpy_my(szText, reinterpret_cast<CONST CHAR*>(&dwNameFlag), 4);
			LOG_C_D(L"dwNameFlag=[%X,%s], dwFlag2=%X", dwNameFlag, MyTools::CCharacter::UTF8ToUnicode(szText).c_str(), dwFlag2);

			DWORD dwNamePtr = GetNamePtr(dwNameFlag, dwCALLAddr);
			if (dwNamePtr != 0)
			{
				LOG_C_D(L"Name=%s", MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(dwNamePtr))).c_str());
			}

			DWORD dwItemDetailPtr = GetItemDetail(dwNameFlag, dwItemDetailCALLAddr);
			if (dwItemDetailPtr != NULL)
			{
				LOG_C_D(L"ItemDetail=%s", MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(dwItemDetailPtr))).c_str());
			}
		}

		/*LOG_C_D(L"-------------------------------------------------");
		for (int i = 0x31/ *1* /;i <= 0x5A/ *Z* /; ++i )
		{
		for (int j = 0x31; j <= 0x5A; ++j)
		{
		DWORD dwNameFlag = 0x49300000; // I0??
		dwNameFlag |= i << 8; //
		dwNameFlag |= j;

		CHAR szText[32] = { 0 };
		MyTools::CCharacter::strcpy_my(szText, reinterpret_cast<CONST CHAR*>(&dwNameFlag), 4);
		LOG_C_D(L"dwNameFlag=[%X,%s]", dwNameFlag, MyTools::CCharacter::UTF8ToUnicode(szText).c_str());
		DWORD dwNamePtr = GetNamePtr(dwNameFlag, dwCALLAddr);
		if (dwNamePtr != 0)
		{
		LOG_C_D(L"Name=%s", MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(dwNamePtr))).c_str());
		}

		DWORD dwItemDetailPtr = GetItemDetail(dwNameFlag, dwItemDetailCALLAddr);
		if (dwItemDetailPtr != NULL)
		{
		LOG_C_D(L"ItemDetail=%s", MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(dwItemDetailPtr))).c_str());
		}
		}
		}*/
	});
}

DWORD GetItemObject_By_Index(_In_ DWORD dwItemIndex, _In_ DWORD dwCALL)
{
	__try
	{
		DWORD dwRetCode = 0;

		__asm
		{
			PUSHAD;
			PUSH dwItemIndex;
			MOV ECX, dwHeroObjectAddr;
			MOV EAX, dwCALL;
			CALL EAX;
			MOV dwRetCode, EAX;
			POPAD;
		}

		return dwRetCode;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		__asm PUSHAD;
		LOG_C_E(L"GetItemObject_By_Index Exception, dwItemIndex=%X,dwCALL", dwItemIndex, dwCALL);
		__asm POPAD;
	}
	return 0;
}

VOID CExpr::ChangeItem(_In_ CONST std::vector<std::wstring>& Vec)
{
	if (Vec.size() != 2)
	{
		LOG_C_E(L"ChangeItem(ItemIndex,ItemId)");
		return;
	}
	DWORD dwItemIndex = std::stoi(Vec.at(0));
	DWORD dwItemId = std::stoi(Vec.at(1), 0, 16);

	LOG_C_D(L"dwItemIndex=%d, dwItemId=%X", dwItemIndex, dwItemId);

	static DWORD dwCALL = MyTools::CLSearchBase::FindCALL("8B442414508BCBE8", 0x6F36A671 - 0x6F36A678, (DWORD)(::GetModuleHandleW(L"Game.dll")), 1, 0, L"Game.dll");
	if (dwCALL == 0)
	{
		LOG_C_E(L"dwCALL = 0");
		return;
	}

	DWORD dwItemObject = GetItemObject_By_Index(dwItemIndex, dwCALL);
	if (dwItemObject == NULL)
	{
		LOG_C_E(L"dwItemObject = NULL");
		return;
	}

	MyTools::CCharacter::WriteDWORD(dwItemObject + 0x30, dwItemId);
}

VOID CExpr::TestPtr(_In_ CONST std::vector<std::wstring>& Vec)
{	
	
	
}
