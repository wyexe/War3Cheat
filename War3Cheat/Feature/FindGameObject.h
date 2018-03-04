#ifndef __WAR3CHEAT_FEATURE_OBJECT_FINDGAMEOBJECT_H__
#define __WAR3CHEAT_FEATURE_OBJECT_FINDGAMEOBJECT_H__

#include <Windows.h>
#include <string>

class CFindGameObject
{
public:
	CFindGameObject() = default;
	~CFindGameObject() = default;

	DWORD	FindSelectedObject();

	DWORD	GetGameObjectAddr(_In_ DWORD dwNodeBase) CONST;

	CONST std::wstring& GetSelectedObjectName() CONST;
private:
	DWORD	GetGameObjectRoot() CONST;

	DWORD	GetGameObjectNameFlag(_In_ DWORD dwNodeBase) CONST;

	VOID	GetGameObjectName(_In_ DWORD dwNodeBase);

	VOID	GetHeroName(_In_ DWORD dwNameObject, _Out_ CHAR* pszText) CONST;

	VOID	GetBuildName(_In_ DWORD dwNameObject, _Out_ CHAR* pszText) CONST;

	VOID	FindGameObject_In_Tree(_In_ DWORD dwTreeNode);
private:
	DWORD			_dwNodeBase = NULL;
	std::wstring	_wsName;
};



#endif // !__WAR3CHEAT_FEATURE_OBJECT_FINDGAMEOBJECT_H__
