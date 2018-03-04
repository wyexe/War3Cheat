#ifndef __LIBTOOLS_LOGLIB_LOGEXPRESSION_H__
#define __LIBTOOLS_LOGLIB_LOGEXPRESSION_H__

#include <Windows.h>
#include <functional>
#include <vector>
#include <string>

namespace libTools
{
	struct ExpressionFunPtr
	{
		std::function<VOID(CONST std::vector<std::wstring>&)> fnPtr;
		std::wstring wsFunName;
	};

	struct ExprStructBase
	{
		virtual VOID Print() = NULL;
	};

	class CExprFunBase
	{
	public:
		CExprFunBase() = default;
		virtual ~CExprFunBase() = default;

		virtual std::vector<ExpressionFunPtr>& GetVec() = NULL;

		virtual VOID Release() = NULL;

		virtual VOID Help(CONST std::vector<std::wstring>&) = NULL;
	};

	class CLogExpression
	{
	public:
		CLogExpression() = default;
		~CLogExpression() = default;

		UINT Push(_In_ std::function<VOID(CONST std::vector<std::wstring>&)> fnPtr, _In_ CONST std::wstring& wsFunName) throw();

		VOID SetVecExprFunPtr(_In_ CONST std::vector<ExpressionFunPtr>& ParmVecFunPtr) throw();

		BOOL Run(_In_ CONST std::wstring& wsText) throw();
	private:
		std::vector<ExpressionFunPtr> VecFunPtr;
	};
}

#endif // !__LIBTOOLS_LOGLIB_LOGEXPRESSION_H__
