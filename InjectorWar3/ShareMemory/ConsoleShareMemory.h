#ifndef __WAR3CHEAT_INJECTORWAR3_SHAREMEMORY_GAMESHAREMEMORY_H__
#define __WAR3CHEAT_INJECTORWAR3_SHAREMEMORY_GAMESHAREMEMORY_H__

#include "ShareMemoryStruct.h"
#include <string>

class CConsoleShareMemory
{
public:
	CConsoleShareMemory() = default;
	~CConsoleShareMemory();

	static CConsoleShareMemory& GetInstance();

	BOOL Create(_In_ HANDLE hStdOut);

	CONST std::wstring& GetErrorText() CONST;

	//
	VOID SetNormalAction(_In_ em_Action_Type emActionType);

	//
	VOID SetActionWithParam(_In_ em_Action_Type emActionType, _In_ CONST std::wstring& wsParam);

	//
	BOOL IsConnectedGame() CONST;
private:
	//
	VOID WaitForGameReadData() CONST;

	VOID Release();
private:
	HANDLE			_hFileMap		= NULL;
	std::wstring	_wsErrorText;
	ShareContent*	_pShareContent	= nullptr;
};


#endif // !__WAR3CHEAT_INJECTORWAR3_SHAREMEMORY_GAMESHAREMEMORY_H__
