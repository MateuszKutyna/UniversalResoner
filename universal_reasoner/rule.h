#ifndef universal_reasoner_rule_h__
#define universal_reasoner_rule_h__

#include "executableWithCost.h"
#include "fact.h"
#include <memory>
#include <functional>
#include <vector>

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
			bool (*comparer)(const FactValue&, const FactValue&) = [](const FactValue& x, const FactValue& y)->bool { return x == y; })
			: compareTo(compareTo), factToCheck(comp2), comparer(comparer) {};
		virtual bool Evaluate()
		{
			return comparer(factToCheck.GetValue(), compareTo->GetValue());
		}
	protected:
		/*const */std::shared_ptr<const LocalFact> compareTo;
		/*const*/ LocalFact factToCheck;
		bool (*comparer)(const FactValue&, const FactValue&);
	};


	class Conclusion
	{
	public:
		virtual void Execute() = 0;
	};

	template<typename T>
	class ConclusionSettingFact : public Conclusion
	{
	public:
		using FactValue = T;
		virtual void Execute() override
		{
			factToBeSet->SetValue(valueToBeSet);
		}

	protected:
		const std::shared_ptr<FactSettable<FactValue>> factToBeSet;
		const FactValue valueToBeSet;
	};

	template<std::invocable T>
	class ConclusionInvokingFunction : public Conclusion
	{
	public:
//		using FactValue = T;
		virtual void Execute() override
		{
			toInvoke();
		}

	protected:
		const T toInvoke();
	};

	template<typename COST>
	class Rule :
		public ExecutableWithCost<COST>
	{
	public: 
		virtual bool CheckAndFire() = 0;
	};

	template<typename COST>
	class RuleImpl : public Rule<COST>
	{
	public:
		virtual bool CheckAndFire() override
		{
			bool allSatisfied = true;
			auto iter = premises.begin();
			while (allSatisfied && (iter != premises.end()))
			{
				allSatisfied = (*iter)->Evaluate();
			}
			if (allSatisfied)
			{
				for (std::shared_ptr<Conclusion> conclusion : conclusions)
				{
					conclusion->Execute();
				}
			} 
			return allSatisfied;
		}

	protected:
		std::vector<std::shared_ptr<Premise>> premises;
		std::vector<std::shared_ptr<Conclusion>> conclusions;
	};
}
#endif // universal_reasoner_rule_h__

