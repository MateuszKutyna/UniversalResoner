#ifndef universal_reasoner_executableWithCost_h__
#define universal_reasoner_executableWithCost_h__

#include "executionCost.h"
#include "fact.h"
#include <vector>

namespace ureasoner
{
	template<typename COST>
	class ExecutableWithCost
	{
	public:
		typedef COST CostType;
		virtual const CostType GetEstimatedCost() const = 0;
		virtual void Fire() const = 0; // or Conclusions to fire?
 		virtual const std::vector<const Fact> GetFactsNeeded() const = 0;
 		virtual const std::vector<const Fact> GetFactsProvided() const = 0;
	};
}

#endif // universal_reasoner_executableWithCost_h__
