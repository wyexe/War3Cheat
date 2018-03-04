#include <stdafx.h>
#include "GameShareMemory.h"
#include <CharacterLib/Character.h>

CGameShareMemory::~CGameShareMemory()
{
	Release();
}

CGameShareMemory& CGameShareMemory::GetInstance()
{
	static CGameShareMemory Instance;
	return Instance;
}

BOOL CGameShareMemory::Run()
{
	_hFileMap = ::OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, SHAREMEMORYNAME);
	if (_hFileMap == NULL)
	{
		return FALSE;
	}


	_pShareContent = reinterpret_cast<ShareContent *>(::MapViewOfFile(_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, NULL, NULL, sizeof(ShareContent)));
	if (_pShareContent == nullptr)
	{
		Release();
		return FALSE;
	}


	return TRUE;
}

BOOL CGameShareMemory::ExistAction(_Out_opt_ CheatActionContent& Content) CONST
{
	if (_pShareContent->emActionType == em_Action_Type::None)
	{
		return FALSE;
	}

	Content.wsParam = _pShareContent->wszParam;
	Content.emActionType = _pShareContent->emActionType;
	_pShareContent->emActionType = em_Action_Type::None;
	return TRUE;
}

LPCWSTR CGameShareMemory::GetConsolePath() CONST
{
	return _pShareContent->wszConsolePath;
}

VOID CGameShareMemory::Release()
{
	if (_pShareContent != nullptr)
	{
		::UnmapViewOfFile(_pShareContent);
		_pShareContent = nullptr;
	}
	if (_hFileMap != NULL)
	{
		::CloseHandle(_hFileMap);
		_hFileMap = NULL;
	}
}

DWORD CGameShareMemory::CheatActionContent::GetChangeItemParamToItemId() CONST
{
	// -0 -I123
	std::wstring wsText = wsParam;
	std::vector<std::wstring> Vec;

	if (!libTools::CCharacter::SplitFormatText(libTools::CCharacter::Trim(wsText), L"-* -*", Vec) || Vec.size() != 2)
	{
		::MessageBoxW(NULL, libTools::CCharacter::MakeFormatText(L"无效的参数[%s]", wsParam.c_str()).c_str(), L"Error", NULL);
		return 0;
	}

	std::string szItemId = libTools::CCharacter::UnicodeToASCII(Vec.at(1));
	if (szItemId.length() != 4)
	{
		::MessageBoxW(NULL, libTools::CCharacter::MakeFormatText(L"无效的物品ID[%s]", Vec.at(1).c_str()).c_str(), L"Error", NULL);
		return 0;
	}

	DWORD dwItemId = 0;
	dwItemId |= szItemId.at(0) << 0x18 & 0xFF;
	dwItemId |= szItemId.at(1) << 0x10 & 0xFF;
	dwItemId |= szItemId.at(2) << 0x08 & 0xFF;
	dwItemId |= szItemId.at(3) & 0xFF;
	return dwItemId;
}

DWORD CGameShareMemory::CheatActionContent::GetChangeItemParamToItemIndex() CONST
{
	// -0 -I123
	std::wstring wsText = wsParam;
	std::vector<std::wstring> Vec;

	if (!libTools::CCharacter::SplitFormatText(libTools::CCharacter::Trim(wsText), L"-* -*", Vec) || Vec.size() != 2)
	{
		::MessageBoxW(NULL, libTools::CCharacter::MakeFormatText(L"无效的参数[%s]", wsParam.c_str()).c_str(), L"Error", NULL);
		return 0;
	}

	return std::stoi(Vec.at(0));
}
