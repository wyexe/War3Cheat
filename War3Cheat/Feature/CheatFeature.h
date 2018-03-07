#ifndef __WAR3CHEAT_WAR3CHEAT_FEATURE_CHEATFEATURE_H__
#define __WAR3CHEAT_WAR3CHEAT_FEATURE_CHEATFEATURE_H__

#include <queue>
#include <functional>
#include <ProcessLib/Memory/Memory.h>
#include <ProcessLib/Memory/SearchBinary.h>
#include "FindGameObject.h"

class CCheatFeature
{
private:
	using PeekMessageAPtr = BOOL(WINAPI*)(_Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax, _In_ UINT wRemoveMsg);

	struct ThreadMethodInfo
	{
		std::function<VOID(VOID)> ThreadExcutePtr;
		HANDLE                    hEvent;
	};

	struct ItemContent
	{
		std::string ItemName;
		std::string ItemDetail;
		std::string	ItemId;

		ItemContent() = default;
		ItemContent(_In_ LPCSTR pszItemName, _In_ LPCSTR pszItemDetail, _In_ LPCSTR pszItemId)
		{
			ItemName = pszItemName;
			ItemDetail = pszItemDetail;
			ItemId = pszItemId;
		}
	};
public:
	CCheatFeature();
	~CCheatFeature();

public:
	///      ------------Feautre---------------------
	VOID SetSelectedObjectInvincible(_In_ DWORD dwSelectedGameObject);

	VOID SetSelectedObjectAttackType(_In_ DWORD dwSelectedGameObject);

	VOID SetSelectedHeroSkillCool(_In_ DWORD dwSelectedGameObject);

	VOID PrintItem();

	VOID ChangeItem(_In_ DWORD dwSelectedGameObject, _In_ DWORD dwItemIndex, _In_ DWORD dwItemId);
public:
	DWORD GetSelectedGameObject();

	BOOL Initialize();

	VOID Release();
private:
	DWORD GetItemObject_By_ItemIndex(_In_ DWORD dwGameObject, _In_ DWORD dwItemIndex);

	DWORD GetNameVirtualTable(_In_ DWORD dwVaildItemFlag);

	DWORD GetItemFlag2ByNameFlag(_In_ DWORD dwNameFlag);

	VOID SetItemNameByNameFlag(_In_ DWORD dwNameFlag, _In_ DWORD* Buffer);

	DWORD GetItemNameByNameFlag(_In_ DWORD dwNameFlag);

	VOID OutputItemName(_In_ CONST std::vector<ItemContent>& Vec) CONST;
private:
	static BOOL WINAPI PeekMessage_(_Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax, _In_ UINT wRemoveMsg);

	static VOID WINAPI PushPtrToMainThread(_In_ std::function<VOID(VOID)> Ptr);
private:
	static std::queue<ThreadMethodInfo> _QueMethodPtr;;
	static CRITICAL_SECTION				_QueCrtSection;
	
private:
	libTools::CSearchBinary _SearchBinary;
	CFindGameObject			_FindGameObject;
	static PeekMessageAPtr	_OldPeekMessagePtr;
};



#endif // !__WAR3CHEAT_WAR3CHEAT_FEATURE_CHEATFEATURE_H__
