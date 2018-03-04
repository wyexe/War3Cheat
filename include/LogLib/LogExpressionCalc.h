#ifndef __LIBTOOLS_LOGLIB_LOGEXPRESSIONCALC_H__
#define __LIBTOOLS_LOGLIB_LOGEXPRESSIONCALC_H__

#include <Windows.h>
#include <stack>
#include <vector>

namespace libTools
{
	class CLogExpressionCalc
	{
	public:
		enum em_Cmd_Type
		{
			em_Cmd_Type_dd,
			em_Cmd_Type_Calc,
			em_Cmd_Type_Function,
			em_Cmd_Type_Struct,
			em_Cmd_Type_Variable,
			em_Cmd_Type_Invalid
		};
	private:
		enum em_Content_Type
		{
			em_Content_Type_None,
			// 1
			em_Content_Type_Number,
			// (
			em_Content_Type_LeftBracket,
			// )
			em_Content_Type_RightBracket,
			// [
			em_Content_Type_LeftAddress,
			// ]
			em_Content_Type_RightAddress,
			// +
			em_Content_Type_Symbol_Add,
			// -
			em_Content_Type_Symbol_Sub,
			// *
			em_Content_Type_Symbol_Mul,
			// /
			em_Content_Type_Symbol_Div,
			// %
			em_Content_Type_Symbol_Mod,
			// ^
			em_Content_Type_Symbol_ExOr,
			// |
			em_Content_Type_Symbol_InOr,
			// ~
			em_Content_Type_Symbol_Comp,
			// &
			em_Content_Type_Symbol_And,
			// <<
			em_Content_Type_Symbol_LeftShift,
			// >>
			em_Content_Type_Symbol_RightShift
		};

		enum em_Text_Type
		{
			em_Text_Type_Invalid,
			em_Text_Type_Number,
			em_Text_Type_Symbol,
			em_Text_Type_Bracket,
			em_Text_Type_NULL
		};

		struct ExpressionInfo
		{
			em_Content_Type emContentType;
			std::wstring wsText;
			UINT_PTR GetHex() CONST
			{
#ifdef _WIN64
				return wcstoll(wsText.c_str(), nullptr, 16);
#else
				return wcstol(wsText.c_str(), nullptr, 16);
#endif // _WIN64


			}
		};

		struct ExpressionContent
		{
			em_Cmd_Type emCmdType;
			std::vector<ExpressionInfo> ExpressionVec;
			UINT uMemWatchCount;
			ExpressionContent() : emCmdType(em_Cmd_Type::em_Cmd_Type_Invalid), uMemWatchCount(NULL)
			{

			}
		};
	public:
		CLogExpressionCalc() = default;
		~CLogExpressionCalc() = default;

		// Analysis Expression
		BOOL Analysis(_In_ CONST std::wstring& wsText) throw();

		BOOL IsConformToCmdType(_In_ CONST std::wstring& wsText) CONST throw();
	private:
		//
		em_Cmd_Type		GetCmdType(_In_ CONST std::wstring& wsText) CONST throw();

		// 
		em_Text_Type	GetTextType(_In_ CONST UINT& uIndex) throw();

		// 
		em_Content_Type GetContentType(_In_ CONST std::wstring& wsSymbolText, _In_ em_Text_Type emTextType) throw();

		// 
		BOOL			GetSymbolText(_In_  _Out_ UINT& uIndex, _Out_ std::wstring& wsSymbolText) throw();

		// 
		BOOL			GetExpression() throw();

		//
		int				GetPriority(_In_ em_Content_Type emContentType) throw();

		//
		BOOL			CompPrioity(_In_ em_Content_Type emTextType1, _In_ em_Content_Type emTextType2) throw();

		//
		BOOL			GetRpn() throw();

		//
		UINT_PTR		CalcResult_By_Parm(_In_ UINT_PTR dwNumberLeft, _In_ UINT_PTR dwNumberRight, _In_ em_Content_Type emSymbolType) throw();

		//
		BOOL			CalcResult_By_Rpn(_Out_opt_ UINT_PTR* pdwResult) throw();

		//
		BOOL			ReadMem_By_Rpn() throw();

		//
		BOOL			IsNumber(_In_ CONST std::wstring& wsText) CONST throw();

	private:
		std::stack<ExpressionInfo>	Rpn;
		ExpressionContent			ExpContent;
		std::wstring				wsExpText;
	};
}

#endif // !__LIBTOOLS_LOGLIB_LOGEXPRESSIONCALC_H__
