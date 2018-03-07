#include <stdafx.h>
#include "ConsoleShareMemory.h"
#include <CharacterLib/Character.h>

CConsoleShareMemory::~CConsoleShareMemory()
{
	Release();
}

CConsoleShareMemory& CConsoleShareMemory::GetInstance()
{
	static CConsoleShareMemory Instance;
	return Instance;
}

BOOL CConsoleShareMemory::Create(_In_ HANDLE hStdOut)
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
	_pShareContent->hStdOut = hStdOut;
	return TRUE;
}

CONST std::wstring& CConsoleShareMemory::GetErrorText() CONST
{
	return _wsErrorText;
}

VOID CConsoleShareMemory::SetNormalAction(_In_ em_Action_Type emActionType)
{
	WaitForGameReadData();
	_pShareContent->emActionType = emActionType;
}

VOID CConsoleShareMemory::SetActionWithParam(_In_ em_Action_Type emActionType, _In_ CONST std::wstring& wsParam)
{
	WaitForGameReadData();
	std::copy(wsParam.begin(), wsParam.end(), _pShareContent->wszParam);
	_pShareContent->emActionType = emActionType;
}

BOOL CConsoleShareMemory::IsConnectedGame() CONST
{
	return _pShareContent->bLive;
}

VOID CConsoleShareMemory::WaitForGameReadData() CONST
{
	for (ULONGLONG ulTick = ::GetTickCount64(); _pShareContent->emActionType != em_Action_Type::None && ::GetTickCount64() - ulTick <= 5 * 1000;)
	{
		::Sleep(100);
	}
}

VOID CConsoleShareMemory::Release()
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
