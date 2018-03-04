#include "stdafx.h"
#include "Expr.h"
#include <tlhelp32.h>
#define _SELF L"Expr.cpp"


#define ReadDWORD(x) MyTools::CCharacter::ReadDWORD(x)

/*
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
		{ std::bind(&CExpr::SetSelectedObjectPowerType,this, std::placeholders::_1) , L"SetSelectedObjectPowerType" },
		{ std::bind(&CExpr::SetSelectedObjectInvincible,this, std::placeholders::_1) , L"SetSelectedObjectInvincible" },
		{ std::bind(&CExpr::TestPtr,this, std::placeholders::_1) , L"TestPtr" },
		{ std::bind(&CExpr::PrintItem,this, std::placeholders::_1) , L"PrintItem" },
		{ std::bind(&CExpr::ChangeItem,this, std::placeholders::_1) , L"ChangeItem" },
		{ std::bind(&CExpr::Help,this, std::placeholders::_1) , L"Help" },
		{ std::bind(&CExpr::SetSelectedHeroSkillCool,this, std::placeholders::_1) , L"SetSelectedHeroSkillCool" },
	};


	return Vec;
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

VOID CExpr::SetSelectedHeroSkillCool(_In_ CONST std::vector<std::wstring>&)
{
	CFindGameObject FindGameObject;
	DWORD dwSelectedObject = FindGameObject.GetGameObjectAddr(FindGameObject.FindSelectedObject());

	MyTools::CLEchoException::GetInstance().InvokeAction(__FUNCTIONW__, [dwSelectedObject]
	{
		DWORD dwCALL = MyTools::CLSearchBase::FindCALL("750433C0EB05E87B112000", 0x6F27752A - 0x6F277530, (DWORD)::GetModuleHandleW(L"Game.dll"), 1, 0, L"Game.dll");
		if (dwCALL == 0)
		{
			LOG_C_E(L"dwCALL = 0");
			return;
		}

		DWORD dwNextSkillAddr = dwSelectedObject + 0x1DC;
		LOG_C_D(L"dwCALL=%X,dwNextSkillAddr=%X", dwCALL, dwNextSkillAddr);
		for (; ;)
		{
			DWORD dwSkillObject = 0;
			__asm
			{
				MOV ECX, dwNextSkillAddr;
				MOV EAX, dwCALL;
				CALL EAX;
				MOV dwSkillObject, EAX;
			}

			if (dwSkillObject == 0)
			{
				break;
			}

			LOG_C_D(L"dwSkillObject=%X", dwSkillObject);
			DWORD dwAddr = ReadDWORD(dwSkillObject + 0x54);
			if (dwAddr != 0)
			{
				DWORD dwCount = ReadDWORD(dwAddr + 0x50);
				DWORD dwSkillArrayAddr = ReadDWORD(dwAddr + 0x54);
				if (ReadDWORD(dwSkillArrayAddr + 0x14) != 0)
				{
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
			}

			dwNextSkillAddr = dwSkillObject + 0x24;
		}
		
	});
}


struct ThreadMethodInfo
{
	std::function<VOID(VOID)> ThreadExcutePtr;
	HANDLE                    hEvent;
};

MyTools::CLLock _LockQueMethodPtr(L"_LockQueMethodPtr");
std::queue<ThreadMethodInfo> _QueMethodPtr;


using PeekMessageAPtr = BOOL(WINAPI*)(_Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax, _In_ UINT wRemoveMsg);
PeekMessageAPtr _OldPeekMessagePtr = nullptr;
BOOL WINAPI PeekMessage_(_Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax, _In_ UINT wRemoveMsg)
{
	BOOL bRetCode = _OldPeekMessagePtr(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
	_LockQueMethodPtr.Access([]
	{
		if (!_QueMethodPtr.empty())
		{

			auto& itm = _QueMethodPtr.front();
			itm.ThreadExcutePtr();
			::SetEvent(itm.hEvent);
			_QueMethodPtr.pop();
		}
	});
	return bRetCode;
}



VOID PushPtrToMainThread(_In_ std::function<VOID(VOID)> MethodPtr)
{
	ThreadMethodInfo ThreadMethodInfo_;
	ThreadMethodInfo_.hEvent = ::CreateEventW(NULL, FALSE, FALSE, NULL);
	if (ThreadMethodInfo_.hEvent == NULL)
	{
		LOG_MSG_CF(L"hEvent = NULL, Err=%d", ::GetLastError());
		return;
	}

	ThreadMethodInfo_.ThreadExcutePtr = MethodPtr;
	_LockQueMethodPtr.Access([ThreadMethodInfo_] {_QueMethodPtr.push(ThreadMethodInfo_); });

	::WaitForSingleObject(ThreadMethodInfo_.hEvent, INFINITE);
	::CloseHandle(ThreadMethodInfo_.hEvent);
	ThreadMethodInfo_.hEvent = INVALID_HANDLE_VALUE;
}



/ *
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
* /

DWORD GetNamePtr(_In_ DWORD dwNameFlag)
{
	static DWORD dwCALLAddr = MyTools::CLSearchBase::FindCALL("E8????????33D28BCBE8????????837C242000", 0x6F36A826 - 0x6F36A82F, (DWORD)(::GetModuleHandleW(L"Game.dll")), 1, 0, L"Game.dll");
	if (dwCALLAddr == 0)
	{
		LOG_C_E(L"dwCALLAddr = 0 ");
		return 0;
	}

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

DWORD GetNameVirtualTable(_In_ DWORD dwVaildItemFlag)
{
	static DWORD dwCALLAddr = MyTools::CLSearchBase::FindCALL("BA????????E8????????85C074??8B48??85", 0x6F32DBE3 - 0x6F32DBE8, (DWORD)(::GetModuleHandleW(L"Game.dll")), 1, 0, L"Game.dll");
	if (dwCALLAddr == 0)
	{
		LOG_C_E(L"dwCALLAddr = 0 ");
		return 0;
	}

	static DWORD dwResNamePtr = MyTools::CLSearchBase::FindBase("BA????????E8????????85C074??8B48??85", 0x6F32DBE3 - 0x6F32DBE3, 1, 0, L"Game.dll", 0xFFFFFFFF);
	if (dwResNamePtr == 0)
	{
		LOG_C_E(L"dwResNamePtr = 0 ");
		return 0;
	}

	DWORD dwValue = 0;
	__asm
	{
		PUSHAD;
		MOV EDX, dwResNamePtr;
		MOV EAX, dwCALLAddr;
		CALL EAX;
		MOV dwValue, EAX;
		POPAD;
	}

	return dwValue != 0 ? ReadDWORD(dwValue) : 0;
}

VOID SetNamePtr(_In_ DWORD dwNameFlag, _In_ DWORD* dwBuffer)
{
	DWORD dwValue = 0;
	__asm
	{
		PUSHAD;
		PUSH dwNameFlag;
		MOV ECX, dwBuffer;
		MOV EAX, DWORD PTR DS : [ECX];
		MOV EAX, DWORD PTR DS : [EAX];
		CALL EAX;
		POPAD;
	}
}

DWORD GetItemFlag2(_In_ DWORD dwNameFlag)
{
	static DWORD dwGetItemFlag2CALL = MyTools::CLSearchBase::FindCALL("B9????????E8????????85C07526", 0x6F32C89A - 0x6F32C88D, (DWORD)(::GetModuleHandleW(L"Game.dll")), 1, 0, L"Game.dll");
	if (dwGetItemFlag2CALL == 0)
	{
		LOG_C_E(L"dwGetItemFlag2CALL = 0 ");
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


VOID CExpr::PrintItem(_In_ CONST std::vector<std::wstring>& Vec)
{
	if (_OldPeekMessagePtr == nullptr)
	{
		MyTools::CLdrHeader LdrHeader;
		LdrHeader.InlineHook(::GetProcAddress(::GetModuleHandleW(L"user32.dll"), "PeekMessageA"), PeekMessage_, reinterpret_cast<void **>(&_OldPeekMessagePtr));
	}

	if (Vec.size() != 1)
	{
		LOG_C_E(L"PrintItem(all|item)");
		return;
	}

	

	static DWORD dwResNamePtr = MyTools::CLSearchBase::FindBase("B9????????E8????????85C07526", 0x6F32D3DA - 0x6F32D3DA, 1, 0, L"Game.dll", 0xFFFFFFFF);
	if (dwResNamePtr == 0)
		return;

	PushPtrToMainThread([=] 
	{
		MyTools::CLEchoException::GetInstance().InvokeAction(__FUNCTIONW__, [=]
		{
			DWORD dwBuffer[0x300] = { 0 };
			DWORD dwNameVirtualTable = 0;

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

				DWORD dwNamePtr = GetNamePtr(dwNameFlag);
				if (dwNamePtr != 0)
				{
					dwNameVirtualTable = GetNameVirtualTable(dwNameFlag);
					if (dwNameVirtualTable == 0)
					{
						CHAR szText[32] = { 0 };
						MyTools::CCharacter::strcpy_my(szText, reinterpret_cast<CONST CHAR*>(&dwNameFlag), 4);
						LOG_C_E(L"dwNameVirtualTable = 0, NameFlag=[%X,%X]", dwNameFlag, MyTools::CCharacter::UTF8ToUnicode(szText).c_str(), dwFlag2);
						continue;
					}

					LOG_C_D(L"dwNameVirtualTable = %X", dwNameVirtualTable);
					break;

					//LOG_C_D(L"Name=%s", MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(dwNamePtr))).c_str());
				}

				/ *DWORD dwItemDetailPtr = GetItemDetail(dwNameFlag, dwItemDetailCALLAddr);
				if (dwItemDetailPtr != NULL)
				{
					LOG_C_D(L"ItemDetail=%s", MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(dwItemDetailPtr))).c_str());
				}

				break;* /
			}


			// 取不了, 因为刷新线程里面有一个TLS的线程局部变量.... 保存了一个不知道什么鬼, 而且还是拿来刷新游戏UI的,导致没办法用...
			LOG_C_D(L"-------------------------------------------------");

			std::vector<int> Vec;
			for (int i = '0'; i <= '9';++i)
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

							dwBuffer[1] = GetItemFlag2(dwNameFlag);
							if (dwBuffer[1] == 0)
								continue;

							dwBuffer[6] = dwNameFlag;
							SetNamePtr(dwNameFlag, dwBuffer);

							if (dwBuffer[10] != 0 && dwBuffer[10] >= 1 && dwBuffer[12] != 0 && dwBuffer[15] == 0 && dwBuffer[38] == 0 && dwBuffer[142] == 0)
							{
								CONST CHAR* pszItemName = reinterpret_cast<CONST CHAR*>(ReadDWORD(dwBuffer[11] + (dwBuffer[10] - 1) * 4));
								CONST CHAR* pszItemDetail = reinterpret_cast<CONST CHAR*>(ReadDWORD(dwBuffer[0x9B] + (dwBuffer[0x9A] - 1) * 4));
								if (pszItemName == nullptr || pszItemDetail == nullptr)
									continue;


								CHAR szText[32] = { 0 };
								DWORD dwItemId = d << 0x18 | c << 0x10 | b << 0x8 | a;
								MyTools::CCharacter::strcpy_my(szText, reinterpret_cast<CONST CHAR*>(&dwItemId), 4);


								LOG_C_D(L"ID=%s, Name=%s", MyTools::CCharacter::UTF8ToUnicode(szText).c_str(), MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(pszItemName))).c_str());
								LOG_C_D(L"ItemDetail=%s", MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(pszItemDetail))).c_str());
							}

						}
					}
				}
			}
			LOG_C_D(L"Over ..........................");
			



			/ *for (int FirstBit = 'A'; FirstBit <= 0x5A; ++FirstBit)
			{
				if (FirstBit >= 0x3A && FirstBit <= 0x40)
					continue;

				for (int SecondBit = 0x30; SecondBit <= 0x5A; ++SecondBit)
				{
					if (SecondBit >= 0x3A && SecondBit <= 0x40)
						continue;

					for (int ThirdBit = 0x30; ThirdBit <= 0x5A; ++ThirdBit)
					{
						if (ThirdBit >= 0x3A && ThirdBit <= 0x40)
							continue;

						for (int FourBit = 0x30; FourBit <= 0x5A; ++FourBit)
						{
							if (FourBit >= 0x3A && FourBit <= 0x40)
								continue;

							DWORD dwNameFlag = 0;
							dwNameFlag |= FirstBit << 0x18;
							dwNameFlag |= SecondBit << 0x10;
							dwNameFlag |= ThirdBit << 0x8;
							dwNameFlag |= FourBit;

							DWORD dwNamePtr = GetNamePtr(dwNameFlag, dwCALLAddr);
							if (dwNamePtr == 0)
								continue;

							CONST CHAR* pszItemName = reinterpret_cast<CONST CHAR*>(dwNamePtr);
							if (*pszItemName == 'D' && MyTools::CCharacter::strcmp_my(pszItemName, "Default string"))
							{
								continue;
							}

							DWORD dwItemDetailPtr = GetItemDetail(dwNameFlag, dwItemDetailCALLAddr);
							if (dwItemDetailPtr == NULL)
								continue;

							CONST CHAR* pszItemDetail = reinterpret_cast<CONST CHAR*>(dwItemDetailPtr);
							if(*pszItemDetail == 'T' && MyTools::CCharacter::strcmp_my(pszItemName, "Tool tip missing!"))
								continue;

							
							CHAR szText[32] = { 0 };
							MyTools::CCharacter::strcpy_my(szText, reinterpret_cast<CONST CHAR*>(&dwNameFlag), 4);
							LOG_C_D(L"dwNameFlag=[%X,%s],Name=%s", dwNameFlag, MyTools::CCharacter::UTF8ToUnicode(szText).c_str(), MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(dwNamePtr))).c_str());
							LOG_C_D(L"ItemDetail=%s", MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(dwItemDetailPtr))).c_str());
						}
					}
				}
			}* /
			

			//            '1'          'Z'
			/ *for (int i = 0x30; i <= 0x5A; ++i)
			{
				for (int j = 0x30; j <= 0x5A; ++j)
				{
					if (i >= 0x3A && i <= 0x40)
						continue;
					if (j >= 0x3A && j <= 0x40)
						continue;

					DWORD dwNameFlag = 0x49300000; // I0??
					dwNameFlag |= i << 8; //
					dwNameFlag |= j;




					DWORD dwNamePtr = GetNamePtr(dwNameFlag, dwCALLAddr);
					if (dwNamePtr != 0)
					{
						CONST CHAR* pszItemName = reinterpret_cast<CONST CHAR*>(dwNamePtr);
						if (*pszItemName == 'D' && MyTools::CCharacter::strcmp_my(pszItemName, "Default string"))
						{
							continue;
						}

						CHAR szText[32] = { 0 };
						MyTools::CCharacter::strcpy_my(szText, reinterpret_cast<CONST CHAR*>(&dwNameFlag), 4);
						LOG_C_D(L"dwNameFlag=[%X,%s],Name=%s", dwNameFlag, MyTools::CCharacter::UTF8ToUnicode(szText).c_str(), MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(dwNamePtr))).c_str());
					}

					DWORD dwItemDetailPtr = GetItemDetail(dwNameFlag, dwItemDetailCALLAddr);
					if (dwItemDetailPtr != NULL)
					{
						LOG_C_D(L"ItemDetail=%s", MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(dwItemDetailPtr))).c_str());
					}
				}
			}* /


		});
	});
	LOG_C_D(L"Print Item ... Over");
}

DWORD GetItemObject_By_Index(_In_ DWORD dwObject, _In_ DWORD dwItemIndex)
{
	static DWORD dwCALL = MyTools::CLSearchBase::FindCALL("8B442414508BCBE8", 0x6F36A671 - 0x6F36A678, (DWORD)(::GetModuleHandleW(L"Game.dll")), 1, 0, L"Game.dll");
	if (dwCALL == 0)
	{
		LOG_C_E(L"dwCALL = 0");
		return 0;
	}

	DWORD dwRetCode = 0;

	__asm
	{
		PUSHAD;
		PUSH dwItemIndex;
		MOV ECX, dwObject;
		MOV EAX, dwCALL;
		CALL EAX;
		MOV dwRetCode, EAX;
		POPAD;
	}

	return dwRetCode;
}

VOID CExpr::ChangeItem(_In_ CONST std::vector<std::wstring>& Vec)
{
	if (Vec.size() != 2)
	{
		LOG_C_E(L"ChangeItem(ItemIndex,ItemId)");
		return;
	}

	if (_OldPeekMessagePtr == nullptr)
	{
		MyTools::CLdrHeader LdrHeader;
		LdrHeader.InlineHook(::GetProcAddress(::GetModuleHandleW(L"user32.dll"), "PeekMessageA"), PeekMessage_, reinterpret_cast<void **>(&_OldPeekMessagePtr));
	}

	DWORD dwItemIndex = std::stoi(Vec.at(0));

	std::string szItemId = MyTools::CCharacter::UnicodeToASCII(Vec.at(1));
	

	DWORD dwItemId = 0;
	dwItemId |= szItemId.at(0) << 0x18;
	dwItemId |= szItemId.at(1) << 0x10;
	dwItemId |= szItemId.at(2) << 0x08;
	dwItemId |= szItemId.at(3);

	LOG_C_D(L"dwItemIndex=%d, dwItemId=%X", dwItemIndex, dwItemId);
	CONST CHAR* pszItemName = nullptr;
	PushPtrToMainThread([&]
	{
		pszItemName = reinterpret_cast<CONST CHAR *>(GetNamePtr(dwItemId));
	});
	if (pszItemName != nullptr)
	{
		LOG_C_D(L"Name=%s", MyTools::CCharacter::UTF8ToUnicode(std::string(reinterpret_cast<CONST CHAR*>(pszItemName))).c_str());
	}


	CFindGameObject FindGameObject;
	DWORD dwItemObject = GetItemObject_By_Index(FindGameObject.GetGameObjectAddr(FindGameObject.FindSelectedObject()), dwItemIndex);
	if (dwItemObject == NULL)
	{
		LOG_C_E(L"dwItemObject = NULL");
		return;
	}

	MyTools::CCharacter::WriteDWORD(dwItemObject + 0x30, dwItemId);
}

VOID CExpr::TestPtr(_In_ CONST std::vector<std::wstring>& Vec)
{	
	
	
}*/
