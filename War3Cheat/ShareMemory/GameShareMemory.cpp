#include <stdafx.h>
#include "GameShareMemory.h"
#include <CharacterLib/Character.h>
#include <LogLib/Log.h>

#define _SELF L"GameShareMemory.cpp"
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


	_pShareContent->bLive = TRUE;
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

	if (!libTools::CCharacter::SplitFormatText(libTools::CCharacter::Trim(wsText), L"* -* -*", Vec) || Vec.size() != 3)
	{
		::MessageBoxW(NULL, libTools::CCharacter::MakeFormatText(L"无效的参数[%s]", wsParam.c_str()).c_str(), L"Error", NULL);
		return 0;
	}


	std::reverse(Vec.at(2).begin(), Vec.at(2).end());
	return *reinterpret_cast<CONST DWORD*>(libTools::CCharacter::UnicodeToASCII(Vec.at(2)).c_str());
}

DWORD CGameShareMemory::CheatActionContent::GetChangeItemParamToItemIndex() CONST
{
	// -0 -I123
	std::wstring wsText = wsParam;
	std::vector<std::wstring> Vec;

	if (!libTools::CCharacter::SplitFormatText(libTools::CCharacter::Trim(wsText), L"* -* -*", Vec) || Vec.size() != 3)
	{
		::MessageBoxW(NULL, libTools::CCharacter::MakeFormatText(L"无效的参数[%s]", wsParam.c_str()).c_str(), L"Error", NULL);
		return 0;
	}

	return std::stoi(Vec.at(1));
}
