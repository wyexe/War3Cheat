#ifndef __LIBTOOLS_PROCESSLIB_COMMON_RESHANDLEMANAGER_H__
#define __LIBTOOLS_PROCESSLIB_COMMON_RESHANDLEMANAGER_H__

#include <functional>

namespace libTools
{
#define SetResDeleter(VarName, Deleter) CResManager<decltype(VarName),std::function<VOID(decltype(VarName)&)>> VarName##Manager(VarName,Deleter)

	template<typename ResHandle, typename Deleter>
	class CResManager
	{
	public:
		CResManager(_In_ ResHandle& ResHandle_, _In_ Deleter fnDeletePtr) : m_ResHandle(ResHandle_), m_fnDeletePtr(fnDeletePtr)
		{

		}
		~CResManager()
		{
			m_fnDeletePtr(m_ResHandle);
		}

		CResManager& operator=(CONST CResManager&) = delete;
	private:
		ResHandle& m_ResHandle;
		Deleter m_fnDeletePtr;
	};
}

#endif // !__LIBTOOLS_PROCESSLIB_COMMON_RESHANDLEMANAGER_H__
