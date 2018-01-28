#ifndef __WAR3CHEAT_FEATURE_EXPR_EXPR_H__
#define __WAR3CHEAT_FEATURE_EXPR_EXPR_H__

#include <MyTools/CLExpression.h>
#include <MyTools/ClassInstance.h>

class CExpr : public MyTools::CExprFunBase, public MyTools::CClassInstance<CExpr>
{
public:
	CExpr() = default;
	virtual ~CExpr() = default;

	virtual VOID Release() override;

	virtual VOID Help(CONST std::vector<std::wstring>& Vec) override;

	virtual std::vector<MyTools::ExpressionFunPtr>& GetVec() override;
private:
	VOID PrintSelectedObject(_In_ CONST std::vector<std::wstring>&);

	VOID SetHeroPowerType(_In_ CONST std::vector<std::wstring>& Vec);

	VOID SetSelectedObjectInvincible(_In_ CONST std::vector<std::wstring>& Vec);

	VOID CloseSkillCool(_In_ CONST std::vector<std::wstring>& Vec);

	VOID TestPtr(_In_ CONST std::vector<std::wstring>&);
};



#endif // !__WAR3CHEAT_FEATURE_EXPR_EXPR_H__
