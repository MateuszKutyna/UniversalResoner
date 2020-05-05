#ifndef universal_reasoner_executableWithCost_h__
#define universal_reasoner_executableWithCost_h__

#include "executionCost.h"
#include <concepts>
#include <vector>

namespace ureasoner
{

	template <class T>	// This is a copy from std. See the comment below
	concept totally_ordered =
		std::equality_comparable<T> &&
		requires(const std::remove_reference_t<T> & a,
			const std::remove_reference_t<T> & b) {
				{ a < b }->std::boolean;
				{ a > b }->std::boolean;
				{ a <= b }->std::boolean;
				{ a >= b }->std::boolean;
	};

	//template<std::totally_ordered COST>
	template<totally_ordered COST> //This is a place for the concept from std, but VS2019 16.5.4 throws internal compiler error
	class ExecutableWithCost
	{
	public:
		typedef COST CostType;
		virtual ~ExecutableWithCost() noexcept{};
		virtual const CostType GetEstimatedCost() const = 0;
		virtual void SetCost(const CostType& cost) { this->cost = cost; }
	protected:
		virtual CostType GetCost() const { return cost; };
		CostType cost{ 0 };
	};
}

#endif // universal_reasoner_executableWithCost_h__
