#ifndef __WAR3CHEAT_INJECTORWAR3_SHAREMEMORY_SHAREMEMORYSTRUCT_H__
#define __WAR3CHEAT_INJECTORWAR3_SHAREMEMORY_SHAREMEMORYSTRUCT_H__

#include <Windows.h>

#define SHAREMEMORYNAME	L"War3CheatShareMemoryName"

enum em_Action_Type
{
	None,
	SetSelectedObjectInvincible,
	SetSelectedObjectAttackType,
	SetSelectedHeroSkillCool,
	PrintItem,
	ChangeItem
};

struct ShareContent
{
	HANDLE hStdOut;
	em_Action_Type emActionType;
	WCHAR wszParam[MAX_PATH];
	WCHAR wszConsolePath[MAX_PATH];
	BOOL  bLive;
};

#endif // !__WAR3CHEAT_INJECTORWAR3_SHAREMEMORY_SHAREMEMORYSTRUCT_H__
