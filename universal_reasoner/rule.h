#ifndef universal_reasoner_rule_h__
#define universal_reasoner_rule_h__

#include "executableWithCost.h"
#include "fact.h"
#include <memory>
#include <functional>
#include <vector>

using std::shared_ptr;
using std::unique_ptr;
using std::vector;

namespace ureasoner
{
	class Premise
	{
	public:
		virtual bool Evaluate() const = 0 ;
		virtual bool Evaluate() = 0;
		virtual shared_ptr<CheckableFact> GetFact() const = 0;
	};

	template<typename T, template<typename> typename FACT_PROVIDER = FactWithGet>
	class PremiseWithType : public Premise
	{
	public:
		template<typename TF>
		using FactProvider = FACT_PROVIDER<TF>;
		typedef FactProvider<T> LocalFact;
		using FactValue = T;

		PremiseWithType(const std::shared_ptr<LocalFact> compareLeft, const FactValue& compareRight,
			bool (*constComparer)(const FactValue&, const FactValue&) = [](const FactValue& x, const FactValue& y)->bool { return x == y; },
			bool (*comparer)(FactValue&, FactValue&) = [](FactValue& x, FactValue& y)->bool { return x == y; })
			: compareLeft(compareLeft), compareRight(std::make_unique<FactConst<T>>(compareRight)), comparer(comparer), constComparer(constComparer) {}
		virtual bool Evaluate() const override
		{
			return constComparer(compareLeft->GetValue(), compareRight->GetValue());
		}
		virtual bool Evaluate() override
		{
			auto r = compareRight->GetValueShared();
			auto l = compareLeft->GetValueShared();
			return comparer(*l, *r);
		}

		virtual shared_ptr<CheckableFact> GetFact() const override
		{
			return compareLeft->GetValueShared();
		}

	protected:
		const std::shared_ptr<LocalFact> compareLeft;
		const std::unique_ptr<LocalFact> compareRight;
		bool (*comparer)(FactValue&, FactValue&);
		bool (*constComparer)(const FactValue&, const FactValue&);
	};

	template<typename T, template<typename> typename FACT_PROVIDER = FactWithGet>
	shared_ptr<PremiseWithType<T, FACT_PROVIDER>> MakePremise(const std::shared_ptr<FACT_PROVIDER<T>> compareLeft, const T& compareRight)
	{
		return make_shared< PremiseWithType<T, FACT_PROVIDER>>(compareLeft, compareRight);
	}

	class Conclusion
	{
	public:
		virtual void Execute() = 0;
		virtual shared_ptr<CheckableFact> GetFact() const = 0;
	};

	template<typename T>
	class ConclusionSettingFact : public Conclusion
	{
	public:
		ConclusionSettingFact(std::shared_ptr<FactSettable<T>> factToSet, const T& valueToBeSet) : factToBeSet(factToSet), valueToBeSet(std::make_unique<T>(valueToBeSet)) {};
		using FactValue = T;
		virtual void Execute() override
		{
			factToBeSet->SetValue(*valueToBeSet);
		}


		virtual shared_ptr<CheckableFact> GetFact() const override
		{
			return factToBeSet->GetValueShared();
		}

	protected:
		std::shared_ptr<FactSettable<FactValue>> factToBeSet;
		const std::unique_ptr<FactValue> valueToBeSet;
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
		virtual vector<shared_ptr<CheckableFact>> GetFactsForFiring() = 0;
		virtual vector<shared_ptr<CheckableFact>> GetFactsConcluding() = 0;
	};

	template<typename COST = double>
	class RuleImpl : public Rule<COST>
	{
	public:
		using CostType = COST;
		RuleImpl(std::vector<std::shared_ptr<Premise>> premises, std::vector<std::shared_ptr<Conclusion>> conclusions) : premises(premises), conclusions(conclusions) {};
		RuleImpl(std::shared_ptr<Premise> premise, std::vector<std::shared_ptr<Conclusion>> conclusions) : premises(std::vector<std::shared_ptr<Premise>>{premise}), conclusions(conclusions) {};
		RuleImpl(std::shared_ptr<Premise> premise, std::shared_ptr<Conclusion> conclusion)
			: premises(std::vector<std::shared_ptr<Premise>>{premise}), 
			conclusions(std::vector<std::shared_ptr<Conclusion>>{conclusion}) {};
		virtual bool CheckAndFire() override
		{
			bool allSatisfied = true;
			auto iter = premises.begin();
			while (allSatisfied && (iter != premises.end()))
			{
				allSatisfied = (*iter++)->Evaluate();
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


		virtual const CostType GetEstimatedCost() const override
		{
			throw std::logic_error("The method or operation is not implemented.");
		}


		virtual vector<shared_ptr<CheckableFact>> GetFactsForFiring() override
		{
			vector<shared_ptr<CheckableFact>> toRet;
			for each (std::shared_ptr<Premise> premise in premises)
			{
				toRet.push_back(premise->GetFact());
			}
		}


		virtual vector<shared_ptr<CheckableFact>> GetFactsConcluding() override
		{
			vector<shared_ptr<CheckableFact>> toRet;
			for each (std::shared_ptr<Conclusion> conclusion in conclusions)
			{
				toRet.push_back(conclusion->GetFact());
			}
		}

	protected:
		std::vector<std::shared_ptr<Premise>> premises;
		std::vector<std::shared_ptr<Conclusion>> conclusions;
	};
}
#endif // universal_reasoner_rule_h__

