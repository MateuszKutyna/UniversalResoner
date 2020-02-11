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
		virtual bool Evaluate() = 0;
	};

	template<typename T>
	class PremiseWithType : public Premise
	{
	public:
		typedef  FactWithValue<T> LocalFact;
		PremiseWithType(std::shared_ptr<const LocalFact> compareTo, /*const bool (*comparer)(const std::shared_ptr<const LocalFact>, */const LocalFact& comp2)
			: compareTo(compareTo), factToCheck(comp2)/*, comparer(comparer) */{};
		virtual bool Evaluate()
		{
			return (factToCheck.isEqual(compareTo));
			//comparer(factToCheck, compareTo);
		}
	protected:
		/*const */std::shared_ptr<const LocalFact> compareTo;
		/*const*/ LocalFact factToCheck;
//		const bool (*comparer)(const std::shared_ptr<const Fact>, const std::shared_ptr < const Fact>);
	};

	template<typename COST>
	class Rule :
		public ExecutableWithCost<COST>
	{
	public:
//		Rule() = 0;
	};
}
#endif // universal_reasoner_rule_h__

