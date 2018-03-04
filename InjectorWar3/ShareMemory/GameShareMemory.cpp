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

BOOL CGameShareMemory::Create()
{
	_hFileMap = ::CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, sizeof(ShareContent), SHAREMEMORYNAME);
	if (_hFileMap == NULL)
	{
		_wsErrorText = libTools::CCharacter::MakeFormatText(L"CreateFileMappingW = NULL, Error=%d", ::GetLastError());
		return FALSE;
	}


	_pShareContent = reinterpret_cast<ShareContent *>(::MapViewOfFile(_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, NULL, NULL, sizeof(ShareContent)));
	if (_pShareContent == nullptr)
	{
		_wsErrorText = libTools::CCharacter::MakeFormatText(L"MapViewOfFile = NULL, Error=%d", ::GetLastError());
		return FALSE;
	}


	ZeroMemory(_pShareContent, sizeof(ShareContent));
	::GetCurrentDirectoryW(MAX_PATH, _pShareContent->wszConsolePath);
	return TRUE;
}

CONST std::wstring& CGameShareMemory::GetErrorText() CONST
{
	return _wsErrorText;
}

VOID CGameShareMemory::SetNormalAction(_In_ em_Action_Type emActionType)
{
	WaitForGameReadData();
	_pShareContent->emActionType = emActionType;
}

VOID CGameShareMemory::SetActionWithParam(_In_ em_Action_Type emActionType, _In_ CONST std::wstring& wsParam)
{
	WaitForGameReadData();
	std::copy(wsParam.begin(), wsParam.end(), _pShareContent->wszParam);
	_pShareContent->emActionType = emActionType;
}

VOID CGameShareMemory::WaitForGameReadData() CONST
{
	for (ULONGLONG ulTick = ::GetTickCount64(); _pShareContent->emActionType != em_Action_Type::None && ::GetTickCount64() - ulTick <= 5 * 1000;)
	{
		::Sleep(100);
	}
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
