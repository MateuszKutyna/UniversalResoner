#ifndef universal_reasoner_rule_h__
#define universal_reasoner_rule_h__

#include "executableWithCost.h"
#include "fact.h"
#include <memory>
#include <functional>

namespace ureasoner
{
	class Premise
	{
	public:
		virtual bool Evaluate() = 0;
	};

	template<typename T, template<typename> typename FACT_PROVIDER = FactWithValue>
	class PremiseWithType : public Premise
	{
	public:
		template<typename TF>
		using FactProvider = FACT_PROVIDER<TF>;
		typedef FactProvider<T> LocalFact;
		using FactValue = T;
		PremiseWithType(std::shared_ptr<const LocalFact> compareTo, 
						const LocalFact& comp2,
			bool (*comparer)(const FactValue&, const FactValue&) /*= &std::equal_to<FactValue>::operator()*/)
			: compareTo(compareTo), factToCheck(comp2), comparer(comparer) {};
		virtual bool Evaluate()
		{
			//return (factToCheck.isEqual(compareTo));
			return comparer(factToCheck.GetValue(), compareTo->GetValue());
		}
	protected:
		/*const */std::shared_ptr<const LocalFact> compareTo;
		/*const*/ LocalFact factToCheck;
		//const std::function<bool(FactValue, FactValue)> comparer;
		bool (*comparer)(const FactValue&, const FactValue&);
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

