#ifndef __WAR3CHEAT_FEATURE_EXPR_EXPR_H__
#define __WAR3CHEAT_FEATURE_EXPR_EXPR_H__

#include <MyTools/CLExpression.h>
#include <MyTools/ClassInstance.h>

class CExpr : public MyTools::CExprFunBase, public MyTools::CClassInstance<CExpr>
{
private:
	using PeekMessageAPtr = BOOL(WINAPI*)(_Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax, _In_ UINT wRemoveMsg);
public:
	CExpr() = default;
	virtual ~CExpr() = default;

	virtual VOID Release() override;

	virtual VOID Help(CONST std::vector<std::wstring>& Vec) override;

	virtual std::vector<MyTools::ExpressionFunPtr>& GetVec() override;
private:
	VOID SetSelectedObjectPowerType(_In_ CONST std::vector<std::wstring>& Vec);

	VOID SetSelectedObjectInvincible(_In_ CONST std::vector<std::wstring>& Vec);

	VOID SetSelectedHeroSkillCool(_In_ CONST std::vector<std::wstring>&);

	VOID PrintItem(_In_ CONST std::vector<std::wstring>& Vec);

	VOID ChangeItem(_In_ CONST std::vector<std::wstring>& Vec);

	VOID TestPtr(_In_ CONST std::vector<std::wstring>&);
private:
};



#endif // !__WAR3CHEAT_FEATURE_EXPR_EXPR_H__
