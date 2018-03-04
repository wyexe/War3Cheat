#ifndef __LIBTOOLS_MATHLIB_DISTANCECALC_H__
#define __LIBTOOLS_MATHLIB_DISTANCECALC_H__

#include <Windows.h>
#include <cmath>
#include <vector>

namespace libTools
{
	class CDistanceCalc
	{
	public:
		CDistanceCalc() = default;
		~CDistanceCalc() = default;

		template<class T>
		static float GetDisBy3D(_In_ CONST T& CurPoint, _In_ CONST T& TarPoint)
		{
			auto fsum = pow(CurPoint.X - TarPoint.X, 2.0f) + pow(CurPoint.Y - TarPoint.Y, 2.0f) + pow(CurPoint.Z - TarPoint.Z, 2.0f);
			return sqrt(fsum);
		}

		template<class T>
		static float GetDisBy2D(_In_ CONST T& CurPoint, _In_ CONST T& TarPoint)
		{
			float fsum = pow(static_cast<float>(CurPoint.X) - static_cast<float>(TarPoint.X), 2.0f) + pow(static_cast<float>(CurPoint.Y) - static_cast<float>(TarPoint.Y), 2.0f);
			return sqrt(fsum);
		}

		template<class T>
		static int GetRecentlyIndexByVec(_In_ CONST std::vector<T>& Vec, _In_ CONST T& TarPoint, _In_ float MinDis = FLT_MAX)
		{
			auto Index = -1;
			auto VecMinDis = FLT_MAX;

			for (UINT i = 0; i < Vec.size(); ++i)
			{
				auto fDis = GetDisBy2D(TarPoint, Vec.at(i));
				if (fDis < VecMinDis)
				{
					VecMinDis = fDis;
					Index = static_cast<int>(i);
				}
			}

			return VecMinDis < MinDis ? Index : -1;
		}

		template<class T, class T2>
		static int GetRecentlyIndexByPointVec(_In_ CONST std::vector<T>& Vec, _In_ CONST T2& TarPoint, _In_ float MinDis = FLT_MAX)
		{
			auto Index = -1;
			auto VecMinDis = FLT_MAX;

			for (UINT i = 0; i < Vec.size(); ++i)
			{
				auto fDis = GetDisBy2D(TarPoint, Vec.at(i).GetPoint());
				if (fDis < VecMinDis)
				{
					VecMinDis = fDis;
					Index = static_cast<int>(i);
				}
			}
			return VecMinDis < MinDis ? Index : -1;
		}
	private:

	};
}

#endif // !__LIBTOOLS_MATHLIB_DISTANCECALC_H__
