#include <stdafx.h>
#include "CheatFeature.h"
#include "../ShareMemory/GameShareMemory.h"
#include <ExceptionLib/Exception.h>
#include <HookLib/InlineHook/InlineHook.h>
#include <CharacterLib/Character.h>
#include <FileLib/File.h>
#include <LogLib/Log.h>

#pragma comment(lib,"ExceptionLib.lib")
#pragma comment(lib,"HookLib.lib")
#pragma comment(lib,"FileLib.lib")

/// Initialize member static variable
std::queue<CCheatFeature::ThreadMethodInfo> CCheatFeature::_QueMethodPtr;;
CRITICAL_SECTION CCheatFeature::_QueCrtSection;
CCheatFeature::PeekMessageAPtr CCheatFeature::_OldPeekMessagePtr = nullptr;

#define _SELF L"CheatFeature.cpp"
CCheatFeature::CCheatFeature()
{

}

CCheatFeature::~CCheatFeature()
{
	Release();
}

VOID CCheatFeature::SetSelectedObjectInvincible(_In_ DWORD dwSelectedGameObject)
{
	/*
	6F37D135   /0F84 DC000000          je      6F37D217
	6F37D13B   |8BCE                   mov     ecx, esi
	6F37D13D   |E8 5E47CCFF            call    6F0418A0
	6F37D142   |85C0                   test    eax, eax
	6F37D144   |0F84 CD000000          je      6F37D217
	6F37D14A   |8B86 80020000          mov     eax, dword ptr [esi+280]         ; 8B??????0000??000100000F??????????F6
	6F37D150   |A9 00010000            test    eax, 100
	6F37D155   |0F85 BC000000          jnz     6F37D217
	6F37D15B   |F646 20 02             test    byte ptr [esi+20], 2             ; InvincibleOffset
	6F37D15F   |0F84 B2000000          je      6F37D217
	6F37D165   |A9 00040000            test    eax, 400
	6F37D16A   |0F85 A7000000          jnz     6F37D217
	6F37D170   |8B16                   mov     edx, dword ptr [esi]
	6F37D172   |8B82 3C010000          mov     eax, dword ptr [edx+13C]
	6F37D178   |8BCE                   mov     ecx, esi
	6F37D17A   |FFD0                   call    eax
	6F37D17C   |85C0                   test    eax, eax
	*/

	static DWORD dwInvincibleOffset = _SearchBinary.FindBase("8B??????0000??000100000F??????????F6", 0x6F37D14A - 0x6F37D15B, 0x2, 0, L"Game.dll", 0xFF);
	if (dwInvincibleOffset == NULL)
	{
		::MessageBoxW(NULL, L"dwInvincibleOffset = 0", L"Error", NULL);
		return;
	}


	// 写入护甲状态=无敌
	// Write Armor to Invincible
	libTools::CMemory::WriteBYTE(dwSelectedGameObject + dwInvincibleOffset, 0xE);
}

VOID CCheatFeature::SetSelectedObjectAttackType(_In_ DWORD dwSelectedGameObject)
{
	/*
	6F353E4C   /75 11                  jnz     short 6F353E5F
	6F353E4E   |8B01                   mov     eax, dword ptr [ecx]
	6F353E50   |8B50 68                mov     edx, dword ptr [eax+68]
	6F353E53   |C781 90000000 01000000 mov     dword ptr [ecx+90], 1            ; C781????????01000000FF??D9
	6F353E5D   |FFD2                   call    edx
	6F353E5F   \D945 00                fld     dword ptr [ebp]
	6F353E62    8B83 E0000000          mov     eax, dword ptr [ebx+E0]          ; EBX=HeroObject
	6F353E68    D806                   fadd    dword ptr [esi]
	6F353E6A    8BBB E4000000          mov     edi, dword ptr [ebx+E4]          ; ArmorOffset
	6F353E70    8D4C24 18              lea     ecx, dword ptr [esp+18]
	6F353E74    51                     push    ecx
	6F353E75    D95C24 2C              fstp    dword ptr [esp+2C]
	6F353E79    8D5424 18              lea     edx, dword ptr [esp+18]
	6F353E7D    D945 00                fld     dword ptr [ebp]
	6F353E80    8D4C24 38              lea     ecx, dword ptr [esp+38]
	6F353E84    D95C24 1C              fstp    dword ptr [esp+1C]
	*/

	static DWORD dwArmorOffset = _SearchBinary.FindBase("C781????????01000000FF??D9", 0x6F353E53 - 0x6F353E6A, 0x2, 0, L"Game.dll");
	if (dwArmorOffset == NULL)
	{
		::MessageBoxW(NULL, L"dwArmorOffset = 0", L"Error", NULL);
		return;
	}

	// 修改护甲=神圣护甲
	// Write Armor to 'Divine Shield'
	DWORD dwHeroArmorAddr = dwSelectedGameObject + dwArmorOffset;
	libTools::CMemory::WriteDWORD(dwHeroArmorAddr, 0x6);

	/*
	6F35FCC2    894424 6C              mov     dword ptr [esp+6C], eax
	6F35FCC6    8B8424 30010000        mov     eax, dword ptr [esp+130]
	6F35FCCD    894C24 70              mov     dword ptr [esp+70], ecx
	6F35FCD1    8B8F 24010000          mov     ecx, dword ptr [edi+124]
	6F35FCD7    33DB                   xor     ebx, ebx                         ; 33??39??????????89??24??89??24??8B
	6F35FCD9    39AF 30010000          cmp     dword ptr [edi+130], ebp
	6F35FCDF    895424 74              mov     dword ptr [esp+74], edx
	6F35FCE3    894424 78              mov     dword ptr [esp+78], eax
	6F35FCE7    8BB1 E8010000          mov     esi, dword ptr [ecx+1E8]         ; PowerTypeOffset1
	6F35FCED    895C24 10              mov     dword ptr [esp+10], ebx
	6F35FCF1    0F85 C5000000          jnz     6F35FDBC
	6F35FCF7    3BF5                   cmp     esi, ebp
	6F35FCF9    0F84 BD000000          je      6F35FDBC
	6F35FCFF    39AE 88000000          cmp     dword ptr [esi+88], ebp
	6F35FD05    74 43                  je      short 6F35FD4A
	
	
	-------------------------------------------------------------------------------
	6F35383C    8BAC24 B8070000        mov     ebp, dword ptr [esp+7B8]
	6F353843    56                     push    esi
	6F353844    8BB424 B8070000        mov     esi, dword ptr [esp+7B8]
	6F35384B    57                     push    edi
	6F35384C    894C24 14              mov     dword ptr [esp+14], ecx
	6F353850    75 11                  jnz     short 6F353863
	6F353852    8B01                   mov     eax, dword ptr [ecx]
	6F353854    8B50 68                mov     edx, dword ptr [eax+68]
	6F353857    C781 90000000 01000000 mov     dword ptr [ecx+90], 1            ; C7??????????01000000FF??8B??????????85
	6F353861    FFD2                   call    edx
	6F353863    8BBE E8010000          mov     edi, dword ptr [esi+1E8]
	6F353869    85FF                   test    edi, edi
	6F35386B    0F84 7F050000          je      6F353DF0
	6F353871    53                     push    ebx
	6F353872    8B9C24 CC070000        mov     ebx, dword ptr [esp+7CC]
	6F353879    83BC9F 88000000 00     cmp     dword ptr [edi+ebx*4+88], 0
	6F353881    0F84 68050000          je      6F353DEF
	6F353887    8B849F F4000000        mov     eax, dword ptr [edi+ebx*4+F4]    ; PowerTypeOffset2
	6F35388E    53                     push    ebx
	6F35388F    8BCF                   mov     ecx, edi
	6F353891    894424 38              mov     dword ptr [esp+38], eax
	6F353895    E8 1634D7FF            call    6F0C6CB0
	6F35389A    2B45 00                sub     eax, dword ptr [ebp]
	6F35389D    53                     push    ebx
	*/

	static DWORD dwPowerTypeOffset1 = _SearchBinary.FindBase("33??39??????????89??24??89??24??8B", 0x6F35FCD7 - 0x6F35FCE7, 0x2, 0x0, L"Game.dll");
	if (dwPowerTypeOffset1 == 0)
	{
		::MessageBoxW(NULL, L"dwPowerTypeOffset1 = 0", L"Error", NULL);
		return;
	}

	static DWORD dwPowerTypeOffset2 = _SearchBinary.FindBase("C7??????????01000000FF??8B??????????85", 0x6F353857 - 0x6F353887, 0x3, 0x0, L"Game.dll");
	if (dwPowerTypeOffset2 == 0)
	{
		::MessageBoxW(NULL, L"dwPowerTypeOffset2 = 0", L"Error", NULL);
		return;
	}

	DWORD dwHeroPowerTypeAddr = libTools::CMemory::ReadDWORD(dwSelectedGameObject + dwPowerTypeOffset1);
	if (dwHeroPowerTypeAddr == 0)
	{
		::MessageBoxW(NULL, L"当前单位不存在攻击类型...", L"Error", NULL);
		return;
	}


	// 修改攻击类型=混乱攻击
	// Write Weaspon Type to 'Chaos Attacks'
	libTools::CMemory::WriteDWORD(dwHeroPowerTypeAddr + dwPowerTypeOffset2, 0x5);

}

VOID CCheatFeature::SetSelectedHeroSkillCool(_In_ DWORD dwSelectedGameObject)
{
	/*
	6F27751E    8B56 28                mov     edx, dword ptr [esi+28]
	6F277521    2356 24                and     edx, dword ptr [esi+24]
	6F277524    8D4E 24                lea     ecx, dword ptr [esi+24]
	6F277527    83FA FF                cmp     edx, -1
	6F27752A    75 04                  jnz     short 6F277530                   ; 750433C0EB05E87B112000
	6F27752C    33C0                   xor     eax, eax
	6F27752E    EB 05                  jmp     short 6F277535
	6F277530    E8 7B112000            call    6F4786B0                         ; GetObjectSkill
	6F277535    85C0                   test    eax, eax
	6F277537    8BF0                   mov     esi, eax
	6F277539  ^ 75 C9                  jnz     short 6F277504
	6F27753B    5F                     pop     edi
	6F27753C    5B                     pop     ebx
	6F27753D    5E                     pop     esi
	6F27753E    C2 0400                retn    4
	*/

	static DWORD dwGetObjectSkillCALL = _SearchBinary.FindCALL("750433C0EB05E87B112000", 0x6F27752A - 0x6F277530, 1, 0, L"Game.dll");
	if (dwGetObjectSkillCALL == NULL)
	{
		::MessageBoxW(NULL, L"dwGetObjectSkillCALL = 0", L"Error", NULL);
		return;
	}

	/*
	6F2774DC    57                     push    edi
	6F2774DD    8B7C24 10              mov     edi, dword ptr [esp+10]
	6F2774E1    33DB                   xor     ebx, ebx
	6F2774E3    8D8E DC010000          lea     ecx, dword ptr [esi+1DC]         ; SkillHeadOffset 8D??????????81??????????0F????23??83
	6F2774E9    81FF 42010D00          cmp     edi, 0D0142
	6F2774EF    0F95C3                 setne   bl
	6F2774F2    2301                   and     eax, dword ptr [ecx]
	6F2774F4    83F8 FF                cmp     eax, -1
	6F2774F7    74 42                  je      short 6F27753B
	6F2774F9    E8 B2112000            call    6F4786B0
	6F2774FE    8BF0                   mov     esi, eax
	6F277500    85F6                   test    esi, esi
	*/
	static DWORD dwSkillHeadOffset = _SearchBinary.FindBase("8D??????????81??????????0F????23??83", 0x6F2774E3 - 0x6F2774E3, 0x2, 0, L"Game.dll");
	if (dwSkillHeadOffset == NULL)
	{
		::MessageBoxW(NULL, L"dwSkillHeadOffset = 0", L"Error", NULL);
		return;
	}

	/*
	6F02E840    56                     push    esi
	6F02E841    57                     push    edi
	6F02E842    8B7C24 0C              mov     edi, dword ptr [esp+C]
	6F02E846    85FF                   test    edi, edi
	6F02E848    8BF1                   mov     esi, ecx
	6F02E84A    7C 34                  jl      short 6F02E880
	6F02E84C    837E 54 00             cmp     dword ptr [esi+54], 0
	6F02E850    75 0D                  jnz     short 6F02E85F
	6F02E852    8B4E 34                mov     ecx, dword ptr [esi+34]
	6F02E855    33D2                   xor     edx, edx
	6F02E857    E8 74762300            call    6F265ED0
	6F02E85C    8946 54                mov     dword ptr [esi+54], eax
	6F02E85F    8B46 54                mov     eax, dword ptr [esi+54]
	6F02E862    85C0                   test    eax, eax
	6F02E864    74 1A                  je      short 6F02E880
	6F02E866    8378 2C 00             cmp     dword ptr [eax+2C], 0            ; 83????0074??39????760f8b
	6F02E86A    74 14                  je      short 6F02E880
	6F02E86C    3978 50                cmp     dword ptr [eax+50], edi
	6F02E86F    76 0F                  jbe     short 6F02E880
	6F02E871    8B40 54                mov     eax, dword ptr [eax+54]
	6F02E874    6BFF 68                imul    edi, edi, 68                     ; struct size
	6F02E877    8B4438 10              mov     eax, dword ptr [eax+edi+10]
	6F02E87B    5F                     pop     edi                              ; 1A800090
	6F02E87C    5E                     pop     esi
	6F02E87D    C2 0400                retn    4
	6F02E880    5F                     pop     edi
	6F02E881    33C0                   xor     eax, eax
	6F02E883    5E                     pop     esi
	6F02E884    C2 0400                retn    4
	*/
	static DWORD dwStructSize = _SearchBinary.FindBase("83????0074??39????760f8b", 0x6F02E866 - 0x6F02E874, 0x2, 0, L"Game.dll", 0xFF);
	if (dwStructSize == NULL)
	{
		::MessageBoxW(NULL, L"dwSkillHeadOffset = 0", L"Error", NULL);
		return;
	}

	libTools::CException::InvokeAction(__FUNCTIONW__, [=] 
	{
		for (DWORD dwNextSkillObjectAddr = dwSelectedGameObject + dwSkillHeadOffset;;)
		{
			DWORD dwSkillObject = NULL;
			__asm
			{
				PUSHAD;
				MOV ECX, dwNextSkillObjectAddr;
				MOV EAX, dwGetObjectSkillCALL;
				CALL EAX;
				MOV dwSkillObject, EAX;
				POPAD;
			}


			if (dwSkillObject == NULL)
			{
				break;
			}


			DWORD dwSkillContentObject = libTools::CMemory::ReadDWORD(dwSkillObject + 0x54);
			if (dwSkillContentObject == NULL)
			{
				continue;
			}


			DWORD dwSkillArrayObjectAddr = libTools::CMemory::ReadDWORD(dwSkillContentObject + 0x54);
			if (dwSkillArrayObjectAddr == 0 || libTools::CMemory::ReadDWORD(dwSkillArrayObjectAddr + 0x14) == 0)
			{
				continue;
			}


			if (libTools::CMemory::ReadFloat(dwSkillArrayObjectAddr + 0x14) > 1.0f/*Skill CD*/ || libTools::CMemory::ReadFloat(dwSkillArrayObjectAddr + 0x10)/*Spent MP*/ > 1)
			{
				// Skill exist difference Level
				DWORD dwSkillArrayCount = libTools::CMemory::ReadDWORD(dwSkillContentObject + 0x50);
				for (DWORD i = 0; i < dwSkillArrayCount; ++i)
				{
					// 耗蓝
					libTools::CMemory::WriteDWORD(dwSkillArrayObjectAddr + i * dwStructSize + 0x10, 0);
					// 技能CD
					libTools::CMemory::WriteFloat(dwSkillArrayObjectAddr + i * dwStructSize + 0x14, 1.0f);
				}
			}
				

			dwNextSkillObjectAddr = dwSkillObject + 0x24;
		}
	});
}

VOID CCheatFeature::PrintItem()
{
	/*
	6F32C892    8BF8                   mov     edi, eax
	6F32C894    8D4424 08              lea     eax, dword ptr [esp+8]
	6F32C898    50                     push    eax
	6F32C899    57                     push    edi
	6F32C89A    B9 D458AB6F            mov     ecx, 6FAB58D4                    ; B9????????E8????????85C07526  ResNamePtr
	6F32C89F    E8 6CCFFEFF            call    6F319810
	6F32C8A4    85C0                   test    eax, eax
	6F32C8A6    75 26                  jnz     short 6F32C8CE
	6F32C8A8    56                     push    esi
	6F32C8A9    50                     push    eax
	*/
	static DWORD dwResNamePtr = _SearchBinary.FindBase("B9????????E8????????85C07526", 0x6F32C89A - 0x6F32C89A, 1, 0, L"Game.dll");
	if (dwResNamePtr == NULL)
	{
		::MessageBoxW(NULL, L"dwResNamePtr = 0", L"Error", NULL);
		return;
	}

	static std::vector<ItemContent> VecItemContent;
	VecItemContent.clear();
	VecItemContent.reserve(1024 * 100);

	ULONGLONG ulTick = ::GetTickCount64();
	PushPtrToMainThread([=] 
	{
		libTools::CException::InvokeAction(__FUNCTIONW__, [=] 
		{
			DWORD dwBuffer[0x300] = { 0 };
			DWORD dwNameVirtualTable = 0;

			DWORD dwNameFlagAddr = libTools::CMemory::ReadDWORD(dwResNamePtr + 0x1C);
			DWORD dwCount = libTools::CMemory::ReadDWORD(dwResNamePtr + 0x24);

			for (DWORD i = 2; i < dwCount; ++i)
			{
				DWORD dwNameFlagObjAddr = libTools::CMemory::ReadDWORD(dwNameFlagAddr + 0xC * i + 0x8);
				if (dwNameFlagObjAddr == 0)
					continue;

				DWORD dwNameFlag = libTools::CMemory::ReadDWORD(dwNameFlagObjAddr + 0x18);
				DWORD dwFlag2 = libTools::CMemory::ReadDWORD(dwNameFlagObjAddr + 0x4);
				if (dwNameFlag == 0 || dwFlag2 == 0)
					continue;

				DWORD dwNamePtr = GetItemNameByNameFlag(dwNameFlag);
				if (dwNamePtr != 0)
				{
					dwNameVirtualTable = GetNameVirtualTable(dwNameFlag);
					if (dwNameVirtualTable == 0)
					{
						//CHAR szText[32] = { 0 };
						//libTools::CCharacter::strcpy_my(szText, reinterpret_cast<CONST CHAR*>(&dwNameFlag), 4);
						continue;
					}

					break;
				}
			}


			std::vector<int> Vec;
			for (int i = '0'; i <= '9'; ++i)
			{
				Vec.push_back(i);
			}
			for (int i = 'A'; i <= 'Z'; ++i)
			{
				Vec.push_back(i);
			}

			for (int a : Vec)
			{
				for (int b : Vec)
				{
					for (int c : Vec)
					{
						for (int d : Vec)
						{
							DWORD dwNameFlag = 0;
							dwNameFlag |= a << 0x18;
							dwNameFlag |= b << 0x10;
							dwNameFlag |= c << 0x8;
							dwNameFlag |= d;


							ZeroMemory(dwBuffer, sizeof(dwBuffer));
							dwBuffer[0] = dwNameVirtualTable;

							dwBuffer[1] = GetItemFlag2ByNameFlag(dwNameFlag);
							if (dwBuffer[1] == 0)
								continue;

							dwBuffer[6] = dwNameFlag;
							SetItemNameByNameFlag(dwNameFlag, dwBuffer);

							if (dwBuffer[10] != 0 && dwBuffer[10] >= 1 && dwBuffer[12] != 0 && dwBuffer[15] == 0 && dwBuffer[38] == 0 && dwBuffer[142] == 0)
							{
								CONST CHAR* pszItemName = reinterpret_cast<CONST CHAR*>(libTools::CMemory::ReadDWORD(dwBuffer[11] + (dwBuffer[10] - 1) * 4));
								CONST CHAR* pszItemDetail = reinterpret_cast<CONST CHAR*>(libTools::CMemory::ReadDWORD(dwBuffer[0x9B] + (dwBuffer[0x9A] - 1) * 4));
								if (pszItemName == nullptr || pszItemDetail == nullptr)
									continue;


								CHAR szItemId[32] = { 0 };
								*reinterpret_cast<DWORD*>(szItemId) = d << 0x18 | c << 0x10 | b << 0x8 | a;
								VecItemContent.emplace_back(pszItemName, pszItemDetail, szItemId);
							}
						}
					}
				}
			}
		});
	});

	LOG_C_D(L"VecItemContent.size=%d, SpentTime=%dms", VecItemContent.size(), static_cast<DWORD>(::GetTickCount64() - ulTick));
	OutputItemName(VecItemContent);
}

VOID CCheatFeature::ChangeItem(_In_ DWORD dwSelectedGameObject, _In_ DWORD dwItemIndex, _In_ DWORD dwItemId)
{
	libTools::CException::InvokeAction(__FUNCTIONW__, [=]
	{
		DWORD dwItemObject = GetItemObject_By_ItemIndex(dwSelectedGameObject, dwItemIndex);
		if (dwItemObject == NULL)
		{
			::MessageBoxW(NULL, L"UnExist Item Object!!!", L"Error", NULL);
			return;
		}

		
		libTools::CMemory::WriteDWORD(dwItemObject + 0x30, dwItemId);
	});
}

DWORD CCheatFeature::GetSelectedGameObject()
{
	return _FindGameObject.GetGameObjectAddr(_FindGameObject.FindSelectedObject());
}

BOOL CCheatFeature::Initialize()
{
	return libTools::CException::InvokeFunc<BOOL>(__FUNCTIONW__, [] 
	{
		::InitializeCriticalSection(&_QueCrtSection);

		if (!libTools::CInlineHook::Hook(reinterpret_cast<LPVOID>(::PeekMessageA), reinterpret_cast<LPVOID>(PeekMessage_), reinterpret_cast<LPVOID *>(&_OldPeekMessagePtr)))
		{
			::MessageBoxW(NULL, L"Hook PeekMessage Faild!!!", L"Error", NULL);
			return FALSE;
		}

		LOG_C_D(L"Initlize....");
		return TRUE;
	});
}

VOID CCheatFeature::Release()
{
	if (_OldPeekMessagePtr != nullptr)
	{
		::DeleteCriticalSection(&_QueCrtSection);

		libTools::CInlineHook::UnHook(reinterpret_cast<LPVOID>(::PeekMessageA), _OldPeekMessagePtr);
		_OldPeekMessagePtr = nullptr;
	}
}

DWORD CCheatFeature::GetItemObject_By_ItemIndex(_In_ DWORD dwGameObject, _In_ DWORD dwItemIndex)
{
	static DWORD dwGetItemObjectByItemIndexCALL = _SearchBinary.FindCALL("8B442414508BCBE8", 0x6F36A671 - 0x6F36A678, 1, 0, L"Game.dll");
	if (dwGetItemObjectByItemIndexCALL == NULL)
	{
		::MessageBoxW(NULL, L"dwGetItemObjectByItemIndexCALL = NULL", L"Error", NULL);
		return 0;
	}


	DWORD dwItemObject = NULL;
	__asm
	{
		PUSHAD;
		PUSH dwItemIndex;
		MOV ECX, dwGameObject;
		MOV EAX, dwGetItemObjectByItemIndexCALL;
		CALL EAX;
		MOV dwItemObject, EAX;
		POPAD;
	}

	return dwItemObject;
}

DWORD CCheatFeature::GetNameVirtualTable(_In_ DWORD dwVaildItemFlag)
{
	/*
	6F32D150    56                     push    esi
	6F32D151    8BF2                   mov     esi, edx
	6F32D153    BA D48C936F            mov     edx, 6F938CD4                    ; BA????????E8????????85C074??8B48??85  dwResNamePtr
	6F32D158    E8 23F7FFFF            call    6F32C880                         ; GetNameVirtualTableCALL
	6F32D15D    85C0                   test    eax, eax
	6F32D15F    74 18                  je      short 6F32D179
	6F32D161    8B48 40                mov     ecx, dword ptr [eax+40]
	6F32D164    85C9                   test    ecx, ecx
	6F32D166    74 11                  je      short 6F32D179
	6F32D168    83C1 FF                add     ecx, -1
	6F32D16B    3BCE                   cmp     ecx, esi
	6F32D16D    72 02                  jb      short 6F32D171
	6F32D16F    8BCE                   mov     ecx, esi
	6F32D171    8B40 44                mov     eax, dword ptr [eax+44]
	6F32D174    8B0488                 mov     eax, dword ptr [eax+ecx*4]
	6F32D177    5E                     pop     esi
	6F32D178    C3                     retn
	6F32D179    33C0                   xor     eax, eax
	6F32D17B    5E                     pop     esi
	6F32D17C    C3                     retn
	*/
	static DWORD dwGetNameVirtualTableCALL = _SearchBinary.FindCALL("BA????????E8????????85C074??8B48??85", 0x6F32D153 - 0x6F32D158, 1, 0, L"Game.dll");
	if (dwGetNameVirtualTableCALL == 0)
	{
		::MessageBoxW(NULL, L"GetNameVirtualTable.dwGetNameVirtualTableCALL = 0", L"", NULL);
		return 0;
	}


	static DWORD dwResNamePtr = _SearchBinary.FindBase("BA????????E8????????85C074??8B48??85", 0x6F32D153 - 0x6F32D153, 1, 0, L"Game.dll", 0xFFFFFFFF);
	if (dwResNamePtr == 0)
	{
		::MessageBoxW(NULL, L"GetNameVirtualTable.dwResNamePtr = 0", L"", NULL);
		return 0;
	}


	DWORD dwValue = 0;
	__asm
	{
		PUSHAD;
		MOV ECX, dwVaildItemFlag;
		MOV EDX, dwResNamePtr;
		MOV EAX, dwGetNameVirtualTableCALL;
		CALL EAX;
		MOV dwValue, EAX;
		POPAD;
	}

	return dwValue != 0 ? libTools::CMemory::ReadDWORD(dwValue) : 0;
}

DWORD CCheatFeature::GetItemFlag2ByNameFlag(_In_ DWORD dwNameFlag)
{
	/*
	6F32C880    51                     push    ecx
	6F32C881    53                     push    ebx
	6F32C882    8BD9                   mov     ebx, ecx
	6F32C884    57                     push    edi
	6F32C885    8D4C24 08              lea     ecx, dword ptr [esp+8]
	6F32C889    895C24 08              mov     dword ptr [esp+8], ebx
	6F32C88D    E8 8EBC1900            call    6F4C8520
	6F32C892    8BF8                   mov     edi, eax
	6F32C894    8D4424 08              lea     eax, dword ptr [esp+8]
	6F32C898    50                     push    eax
	6F32C899    57                     push    edi
	6F32C89A    B9 D458AB6F            mov     ecx, 6FAB58D4                    ; B9????????E8????????85C07526  ResNamePtr
	6F32C89F    E8 6CCFFEFF            call    6F319810
	6F32C8A4    85C0                   test    eax, eax
	6F32C8A6    75 26                  jnz     short 6F32C8CE
	6F32C8A8    56                     push    esi
	6F32C8A9    50                     push    eax
	6F32C8AA    50                     push    eax
	6F32C8AB    57                     push    edi
	6F32C8AC    B9 D458AB6F            mov     ecx, 6FAB58D4
	6F32C8B1    E8 FAECFFFF            call    6F32B5B0
	6F32C8B6    8BF0                   mov     esi, eax
	6F32C8B8    8B16                   mov     edx, dword ptr [esi]
	6F32C8BA    8B02                   mov     eax, dword ptr [edx]
	6F32C8BC    897E 04                mov     dword ptr [esi+4], edi
	6F32C8BF    8B4C24 0C              mov     ecx, dword ptr [esp+C]
	6F32C8C3    894E 18                mov     dword ptr [esi+18], ecx
	6F32C8C6    53                     push    ebx
	6F32C8C7    8BCE                   mov     ecx, esi
	6F32C8C9    FFD0                   call    eax
	6F32C8CB    8BC6                   mov     eax, esi
	6F32C8CD    5E                     pop     esi
	6F32C8CE    5F                     pop     edi
	6F32C8CF    5B                     pop     ebx
	6F32C8D0    59                     pop     ecx
	6F32C8D1    C3                     retn
	*/
	static DWORD dwGetItemFlag2CALL = _SearchBinary.FindCALL("B9????????E8????????85C07526", 0x6F32C89A - 0x6F32C88D, 1, 0, L"Game.dll");
	if (dwGetItemFlag2CALL == 0)
	{
		::MessageBoxW(NULL, L"GetItemFlag2.dwGetItemFlag2CALL = 0", L"", NULL);
		return 0;
	}


	DWORD dwBuffer = dwNameFlag;
	DWORD dwValue = 0;
	__asm
	{
		PUSHAD;
		LEA ECX, dwBuffer;
		MOV EAX, dwGetItemFlag2CALL;
		CALL EAX;
		MOV dwValue, EAX;
		POPAD;
	}

	return dwValue;
}

VOID CCheatFeature::SetItemNameByNameFlag(_In_ DWORD dwNameFlag, _In_ DWORD* Buffer)
{
	__asm
	{
		PUSHAD;
		PUSH dwNameFlag;
		MOV ECX, Buffer;
		MOV EAX, DWORD PTR DS : [ECX];
		MOV EAX, DWORD PTR DS : [EAX];
		CALL EAX;
		POPAD;
	}
}

DWORD CCheatFeature::GetItemNameByNameFlag(_In_ DWORD dwNameFlag)
{
	/*
	6F369CDD    8D4C24 20              lea     ecx, dword ptr [esp+20]
	6F369CE1    51                     push    ecx
	6F369CE2    6A 2A                  push    2A
	6F369CE4    8BC8                   mov     ecx, eax
	6F369CE6    E8 D5BCC9FF            call    6F0059C0                         ; E8????????33D28BCBE8????????837C242000
	6F369CEB    33D2                   xor     edx, edx
	6F369CED    8BCB                   mov     ecx, ebx
	6F369CEF    E8 EC3EFCFF            call    6F32DBE0                         ; GetItemNameByNameFlag
	6F369CF4    837C24 20 00           cmp     dword ptr [esp+20], 0
	6F369CF9    8BE8                   mov     ebp, eax
	6F369CFB    74 11                  je      short 6F369D0E
	6F369CFD    8B4C24 24              mov     ecx, dword ptr [esp+24]
	6F369D01    6A 01                  push    1
	6F369D03    33D2                   xor     edx, edx
	6F369D05    E8 2640FCFF            call    6F32DD30
	6F369D0A    8BD8                   mov     ebx, eax
	6F369D0C    EB 02                  jmp     short 6F369D10
	*/
	static DWORD dwGetItemNameByNameFlagCALL = _SearchBinary.FindCALL("E8????????33D28BCBE8????????837C242000", 0x6F36A826 - 0x6F36A82F, 1, 0, L"Game.dll");
	if (dwGetItemNameByNameFlagCALL == 0)
	{
		::MessageBoxW(NULL, L"GetItemNameByNameFlag.dwGetItemNameByNameFlagCALL = 0", L"", NULL);
		return 0;
	}

	DWORD dwRetCode = 0;
	__asm
	{
		PUSHAD;
		XOR EDX, EDX;
		MOV ECX, dwNameFlag;
		MOV EAX, dwGetItemNameByNameFlagCALL;
		CALL EAX;
		MOV dwRetCode, EAX;
		POPAD;
	}

	return dwRetCode;
}

VOID CCheatFeature::OutputItemName(_In_ CONST std::vector<ItemContent>& Vec) CONST
{
	WCHAR wszPath[MAX_PATH] = { 0 };
	libTools::CCharacter::strcpy_my(wszPath, CGameShareMemory::GetInstance().GetConsolePath());
	::lstrcatW(wszPath, L"\\ItemName.txt");

	if (libTools::CFile::FileExist(wszPath))
	{
		libTools::CFile::ClearFileContent(wszPath);
	}
	else
	{
		libTools::CFile::CreateUnicodeTextFile(wszPath);
	}

	static std::wstring wsText;


	wsText.clear();
	wsText.reserve(1024 * 1000);
	for (CONST auto& itm : Vec)
	{
		wsText += libTools::CCharacter::MakeFormatText(L"[%s]\r\n", libTools::CCharacter::UTF8ToUnicode(itm.ItemId).c_str());
		wsText += libTools::CCharacter::MakeFormatText(L"Name=%s\r\n", libTools::CCharacter::UTF8ToUnicode(itm.ItemName).c_str());
		wsText += libTools::CCharacter::MakeFormatText(L"Detail=%s\r\n", libTools::CCharacter::UTF8ToUnicode(itm.ItemDetail).c_str());
		wsText += L"\r\n";
	}
	libTools::CFile::WriteUnicodeFile(wszPath, wsText);
	LOG_C_D(L"Write File Done!");
}

BOOL WINAPI CCheatFeature::PeekMessage_(_Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax, _In_ UINT wRemoveMsg)
{
	::EnterCriticalSection(&_QueCrtSection);
	if (!_QueMethodPtr.empty())
	{
		_QueMethodPtr.front().ThreadExcutePtr();
		::SetEvent(_QueMethodPtr.front().hEvent);
		_QueMethodPtr.pop();
	}
	::LeaveCriticalSection(&_QueCrtSection);
	return _OldPeekMessagePtr(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}

VOID WINAPI CCheatFeature::PushPtrToMainThread(_In_ std::function<VOID(VOID)> Ptr)
{
	ThreadMethodInfo Info;
	Info.ThreadExcutePtr = Ptr;
	Info.hEvent = ::CreateEventW(NULL, FALSE, FALSE, NULL);
	if (Info.hEvent == NULL)
	{
		::MessageBoxW(NULL, L"CreateEventW = NULL", L"Error", NULL);
		return;
	}


	
	::EnterCriticalSection(&_QueCrtSection);
	_QueMethodPtr.push(Info);
	::LeaveCriticalSection(&_QueCrtSection);


	::WaitForSingleObject(Info.hEvent, INFINITE);
	::CloseHandle(Info.hEvent);
	Info.hEvent = INVALID_HANDLE_VALUE;
}
