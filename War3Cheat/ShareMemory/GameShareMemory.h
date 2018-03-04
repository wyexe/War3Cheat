#ifndef __WAR3CHEAT_WAR3CHEAT_SHAREMEMORY_GAMESHAREMEMORY_H__
#define __WAR3CHEAT_WAR3CHEAT_SHAREMEMORY_GAMESHAREMEMORY_H__

#include <string>
#include "../InjectorWar3/ShareMemory/ShareMemoryStruct.h"


class CGameShareMemory
{
public:
	struct CheatActionContent
	{
		em_Action_Type emActionType;
		std::wstring   wsParam;

		DWORD GetChangeItemParamToItemId() CONST;

		DWORD GetChangeItemParamToItemIndex() CONST;
	};
public:
	CGameShareMemory() = default;
	~CGameShareMemory();

	static CGameShareMemory& GetInstance();

	BOOL Run();

	BOOL ExistAction(_Out_opt_ CheatActionContent& Content) CONST;

	LPCWSTR GetConsolePath() CONST;

	VOID Release();
private:
	
private:
	HANDLE			_hFileMap		= NULL;
	ShareContent*	_pShareContent	= nullptr;
};


#endif // !__WAR3CHEAT_WAR3CHEAT_SHAREMEMORY_GAMESHAREMEMORY_H__
