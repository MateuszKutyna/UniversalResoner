#ifndef universal_reasoner_rule_h__
#define universal_reasoner_rule_h__

#include "executableWithCost.h"
#include "fact.h"
#include <memory>

namespace ureasoner
{
	class Premise
	{
	public:
		Premise(const Fact compareTo, const std::shared_ptr<const Fact> factToCheck, const bool (*comparer)(const std::shared_ptr<const Fact>, const Fact&))
			: compareTo(compareTo), factToCheck(factToCheck), comparer(comparer) {};
		bool Evaluate()
		{
			return comparer(factToCheck, compareTo);
		}
	protected:
		const Fact compareTo;
		const std::shared_ptr<const Fact> factToCheck;
		const bool (*comparer)(const std::shared_ptr<const Fact>, const Fact&);
	};

	template<typename COST>
	class Rule :
		public ExecutableWithCost<COST>
	{
	public:
		Rule()
	};
}
#endif // universal_reasoner_rule_h__

