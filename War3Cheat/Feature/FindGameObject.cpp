#include <stdafx.h>
#include "FindGameObject.h"
#include <ProcessLib/Memory/SearchBinary.h>
#include <ProcessLib/Memory/Memory.h>
#include <CharacterLib/Character.h>
#include <LogLib/Log.h>

#pragma comment(lib,"ProcessLib.lib")
#pragma comment(lib,"LogLib.lib")

#define _SELF L"FindGameObject.cpp"

#define ReadDWORD(x) libTools::CMemory::ReadDWORD(x)

DWORD CFindGameObject::FindSelectedObject()
{
	FindGameObject_In_Tree(ReadDWORD(GetGameObjectRoot()));
	return _dwNodeBase;
}

DWORD CFindGameObject::GetGameObjectRoot() CONST
{
	/*
	6F5FE610  |.  D805 B85A876F fadd    dword ptr [6F875AB8]
	6F5FE616  |>  DEF9          fdivp   st(1), st
	6F5FE618  |.  D91D 60E6AC6F fstp    dword ptr [6FACE660]
	6F5FE61E  |.  EB 02         jmp     short 6F5FE622                   ;  EB02DDD88B0D????????3B
	6F5FE620  |>  DDD8          fstp    st
	6F5FE622  |>  8B0D 6CE6AC6F mov     ecx, dword ptr [6FACE66C]        ;  dwGameObjectRoot
	6F5FE628  |.  3BCE          cmp     ecx, esi
	6F5FE62A  |.  74 59         je      short 6F5FE685
	6F5FE62C  |.  8B11          mov     edx, dword ptr [ecx]
	6F5FE62E  |.  8B42 24       mov     eax, dword ptr [edx+24]
	6F5FE631  |.  68 9CE6AC6F   push    6FACE69C
	*/
	static DWORD dwGameObjectRoot = NULL;
	if (dwGameObjectRoot == NULL)
	{
		dwGameObjectRoot = libTools::CSearchBinary().FindBase("EB02DDD88B0D????????3B", 0x267EDBE - 0x267EDC2, 2, 0, L"Game.dll");
		if (dwGameObjectRoot == NULL)
		{
			LOG_C_E(L"dwGameObjectRoot=NULL");
			return 0;
		}
	}

	return dwGameObjectRoot;
}

DWORD CFindGameObject::GetGameObjectAddr(_In_ DWORD dwNodeBase) CONST
{
	return ReadDWORD(ReadDWORD(ReadDWORD(dwNodeBase + 0x3C4) + 0x130) + 0x124);
}

CONST std::wstring& CFindGameObject::GetSelectedObjectName() CONST
{
	return _wsName;
}

DWORD CFindGameObject::GetGameObjectNameFlag(_In_ DWORD dwNodeBase) CONST
{
	/*
	6F346A20  /$  81EC 04020000 sub     esp, 204
	6F346A26  |.  A1 40E1AA6F   mov     eax, dword ptr [6FAAE140]
	6F346A2B  |.  33C4          xor     eax, esp
	6F346A2D  |.  898424 000200>mov     dword ptr [esp+200], eax
	6F346A34  |.  56            push    esi
	6F346A35  |.  57            push    edi
	6F346A36  |.  8B79 30       mov     edi, dword ptr [ecx+30]
	6F346A39  |.  8BC7          mov     eax, edi
	6F346A3B  |.  C1E8 18       shr     eax, 18
	6F346A3E  |.  83C0 BF       add     eax, -41
	6F346A41  |.  83F8 19       cmp     eax, 19
	6F346A44  |.  8BF2          mov     esi, edx
	6F346A46  |.  77 15         ja      short 6F346A5D
	6F346A48  |.  68 00020000   push    200
	6F346A4D  |.  8D4424 0C     lea     eax, dword ptr [esp+C]
	6F346A51  |.  50            push    eax
	6F346A52  |.  E8 C980F2FF   call    6F26EB20                         ;  HeroName CALL
	6F346A57  |.  8D4424 08     lea     eax, dword ptr [esp+8]
	6F346A5B  |.  EB 09         jmp     short 6F346A66
	6F346A5D  |>  33D2          xor     edx, edx
	6F346A5F  |.  8BCF          mov     ecx, edi
	6F346A61  |.  E8 7A71FEFF   call    6F32DBE0                         ;  Build Name CALL
	6F346A66  |>  8BF8          mov     edi, eax
	6F346A68  |.  E8 23FFFFFF   call    6F346990
	6F346A6D  |.  8B8C24 080200>mov     ecx, dword ptr [esp+208]
	6F346A74  |.  5F            pop     edi
	6F346A75  |.  5E            pop     esi
	6F346A76  |.  33CC          xor     ecx, esp
	6F346A78  |.  E8 DCA54900   call    6F7E1059
	6F346A7D  |.  81C4 04020000 add     esp, 204
	6F346A83  \.  C3            retn
	*/
	return ReadDWORD(GetGameObjectAddr(dwNodeBase) + 0x30) >> 0x18;
}

VOID CFindGameObject::GetGameObjectName(_In_ DWORD dwNodeBase)
{
	/*
	-------------------------------------------------------------------------------------
	6F35FA31   .  8078 30 0C    cmp     byte ptr [eax+30], 0C
	6F35FA35   .  73 0F         jnb     short 6F35FA46
	6F35FA37   .  8D8B F0000000 lea     ecx, dword ptr [ebx+F0]
	6F35FA3D   .  E8 2E371100   call    6F473170
	6F35FA42   .  3BC5          cmp     eax, ebp
	6F35FA44   .  75 06         jnz     short 6F35FA4C
	6F35FA46   >  896C24 18     mov     dword ptr [esp+18], ebp
	6F35FA4A   .  EB 08         jmp     short 6F35FA54
	6F35FA4C   >  C74424 18 010>mov     dword ptr [esp+18], 1
	6F35FA54   >  8B8F 24010000 mov     ecx, dword ptr [edi+124]         ;  .......
	6F35FA5A   .  8B97 28010000 mov     edx, dword ptr [edi+128]
	6F35FA60   .  E8 BB6FFEFF   call    6F346A20                         ;  1
	6F35FA65   .  8B5C24 14     mov     ebx, dword ptr [esp+14]
	6F35FA69   .  3BDD          cmp     ebx, ebp
	6F35FA6B   .  74 3A         je      short 6F35FAA7
	6F35FA6D   .  8B8F 24010000 mov     ecx, dword ptr [edi+124]
	6F35FA73   .  E8 187CF1FF   call    6F277690
	6F35FA78   .  3BF5          cmp     esi, ebp
	6F35FA7A   .  74 2B         je      short 6F35FAA7

	---------------------------------------------------------------------------------
	6F35EA38  |> /6A 01         push    1
	6F35EA3A  |. |8BCD          mov     ecx, ebp
	6F35EA3C  |. |E8 4F7AFEFF   call    6F346490
	6F35EA41  |. |8B8D 30010000 mov     ecx, dword ptr [ebp+130]         ;  ....
	6F35EA47  |. |8B11          mov     edx, dword ptr [ecx]
	6F35EA49  |. |8B42 78       mov     eax, dword ptr [edx+78]
	6F35EA4C  |. |6A 01         push    1
	6F35EA4E  |. |56            push    esi
	6F35EA4F  |. |FFD0          call    eax                              ;  Name3
	6F35EA51  |. |E9 AB020000   jmp     6F35ED01
	6F35EA56  |> |3BFB          cmp     edi, ebx
	6F35EA58  |.^ 74 DE         je      short 6F35EA38
	6F35EA5A  |> |6A 01         push    1
	6F35EA5C  |. |6A 01         push    1
	6F35EA5E  |. |53            push    ebx

	---------------------------------------------------------------------------------
	6F2F8574  |.  E8 C7AA0700   call    6F373040
	6F2F8579  |.  C786 84020000>mov     dword ptr [esi+284], 0
	6F2F8583  |>  8B8E C8030000 mov     ecx, dword ptr [esi+3C8]
	6F2F8589  |.  E8 C2A90700   call    6F372F50
	6F2F858E  |.  8B8E C4030000 mov     ecx, dword ptr [esi+3C4]         ;  ...............
	6F2F8594  |.  E8 E7610600   call    6F35E780                         ;  Name 4
	6F2F8599  |.  8BCD          mov     ecx, ebp
	6F2F859B  |.  E8 70741200   call    6F41FA10
	6F2F85A0  |.  50            push    eax
	6F2F85A1  |.  8D4C24 18     lea     ecx, dword ptr [esp+18]
	6F2F85A5  |.  E8 E602D4FF   call    6F038890
	6F2F85AA  |.  8BCD          mov     ecx, ebp
	
	*/
	DWORD dwNameObject = GetGameObjectAddr(dwNodeBase);
	if (dwNameObject == NULL)
	{
		LOG_C_E(L"UnExist Name[%X]", dwNodeBase);
		return;
	}

	CHAR szText[0x200] = { 0 };

	DWORD dwNameFlag = GetGameObjectNameFlag(dwNodeBase);
	if (dwNameFlag - 0x41 < 0x19)
	{
		GetHeroName(dwNameObject, szText);
	}
	else
	{
		GetBuildName(dwNameObject, szText);
	}

	if (*szText == '\0')
	{
		LOG_C_E(L"Empty Name");
		return;
	}


	///     Selected Hero|Build 
	_dwNodeBase = dwNodeBase;
	_wsName = libTools::CCharacter::UTF8ToUnicode(szText);
}

VOID CFindGameObject::GetHeroName(_In_ DWORD dwNameObject, _Out_ CHAR* pszText) CONST
{
	static DWORD dwGameObjectNameCALL = NULL;
	if (dwGameObjectNameCALL == NULL)
	{
		dwGameObjectNameCALL = libTools::CSearchBinary().FindCALL("68000200008D44240C", 0x26D7588 - 0x26D7592, 1, 0, L"Game.dll");
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
			MOV EAX, pszText;
			PUSH EAX;
			MOV ECX, dwNameObject;
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

VOID CFindGameObject::GetBuildName(_In_ DWORD dwNameObject, _Out_ CHAR* pszText) CONST
{
	static DWORD dwGameObjectNameCALL = NULL;
	if (dwGameObjectNameCALL == NULL)
	{
		dwGameObjectNameCALL = libTools::CSearchBinary().FindCALL("68000200008D44240C", 0x6F346A48 - 0x6F346A61, 1, 0, L"Game.dll");
		if (dwGameObjectNameCALL == NULL)
		{
			LOG_C_E(L"dwGameObjectNameCALL=NULL");
			return;
		}
	}

	__try
	{
		DWORD dwNamePtr = NULL;
		_asm
		{
			PUSHAD;
			MOV ECX, dwNameObject;
			MOV EDI, DWORD PTR DS : [ECX + 0x30];
			MOV ECX, EDI;
			MOV EAX, dwGameObjectNameCALL;
			CALL EAX;
			MOV dwNamePtr, EAX;
			POPAD;
		}

		if (dwNamePtr != NULL && ReadDWORD(dwNamePtr) != NULL)
		{
			libTools::CCharacter::strcpy_my(pszText, reinterpret_cast<CONST CHAR*>(dwNamePtr), 32);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		LOG_C_E(L"Exception GetBuildName");
	}
}

VOID CFindGameObject::FindGameObject_In_Tree(_In_ DWORD dwTreeNode)
{
	if ((ReadDWORD(dwTreeNode + 0xB0) & 0x3) == 0 && GetGameObjectAddr(dwTreeNode) != NULL)
	{
		GetGameObjectName(dwTreeNode);
		LOG_C_D(L"[%s].dwNodeBase=%X, ObjectAddr=%X", _wsName.c_str(), dwTreeNode, GetGameObjectAddr(dwTreeNode));
	}

	int nNextNodeBase = static_cast<int>(ReadDWORD(dwTreeNode + 0x1C));
	while (nNextNodeBase > 0 && _dwNodeBase == NULL)
	{
		FindGameObject_In_Tree(ReadDWORD(nNextNodeBase + 0xC));
		nNextNodeBase = static_cast<int>(ReadDWORD(nNextNodeBase + 0x8));
	}
}

