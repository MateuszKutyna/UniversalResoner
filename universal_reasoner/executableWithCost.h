#ifndef universal_reasoner_executableWithCost_h__
#define universal_reasoner_executableWithCost_h__

#include "executionCost.h"
#include <concepts>
#include <vector>

namespace ureasoner
{
	template <class T>
	concept equality_comparable =
		requires(const std::remove_reference_t<T> &x, const std::remove_reference_t<T> &y) {
			{ x == y };
			{ x != y };
	};

	template <class T>
	concept totally_ordered =
		equality_comparable<T> &&
		requires(const std::remove_reference_t<T> & a,
			const std::remove_reference_t<T> & b) {
				{ a < b };
				{ a > b };
				{ a <= b };
				{ a >= b };
	};


	template<totally_ordered COST> 
	class ExecutableWithCost
	{
	public:
		typedef COST CostType;
		//ExecutableWithCost(const CostType& cost) :cost(cost) {}
		virtual ~ExecutableWithCost() noexcept = default;
		virtual CostType GetEstimatedCost() const = 0;
		virtual void SetCost(const CostType& cost) { this->cost = cost; }
	protected:
		virtual CostType GetCost() const { return cost; };
		CostType cost{ 0 };
	};
}

#endif // universal_reasoner_executableWithCost_h__
