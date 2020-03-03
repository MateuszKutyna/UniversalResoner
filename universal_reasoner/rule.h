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
	template<typename COST = double>
	class Premise : public ExecutableWithCost<COST>
	{
	public:
		virtual bool Evaluate() const = 0 ;
		virtual bool Evaluate() = 0;
		virtual const shared_ptr<CheckableFact<COST>> GetFact() const = 0;
	};

	template<typename T, template<typename> typename FACT_PROVIDER = FactWithGet, typename COST = double>
	class PremiseWithType : public Premise<COST>
	{
	public:
		template<typename TF>
		using FactProvider = FACT_PROVIDER<TF>;
		typedef FactProvider<T> LocalFact;
		using FactValue = T;

		PremiseWithType(const std::shared_ptr<LocalFact> compareLeft, const FactValue& compareRight, const COST& cost = 0,
			bool (*constComparer)(const FactValue&, const FactValue&) = [](const FactValue& x, const FactValue& y)->bool { return x == y; },
			bool (*comparer)(FactValue&, FactValue&) = [](FactValue& x, FactValue& y)->bool { return x == y; })
			: compareLeft(compareLeft), compareRight(std::make_unique<FactConst<T>>(compareRight)), comparer(comparer), constComparer(constComparer), cost(cost) {};

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

		virtual const shared_ptr<CheckableFact<COST>> GetFact() const override
		{
			return compareLeft;
		}


		virtual const COST GetEstimatedCost() const override
		{
			return cost + compareLeft->GetEstimatedCost() + compareRight->GetEstimatedCost();
		}

	protected:
		const std::shared_ptr<LocalFact> compareLeft;
		const std::unique_ptr<LocalFact> compareRight;
		bool (*comparer)(FactValue&, FactValue&);
		bool (*constComparer)(const FactValue&, const FactValue&);
		COST cost;
	};

	template<typename T, template<typename> typename FACT_PROVIDER = FactWithGet>
	shared_ptr<PremiseWithType<T, FACT_PROVIDER>> MakePremise(const std::shared_ptr<FACT_PROVIDER<T>> compareLeft, const T& compareRight)
	{
		return make_shared< PremiseWithType<T, FACT_PROVIDER>>(compareLeft, compareRight);
	}

	template< typename COST = double>
	class Conclusion : public ExecutableWithCost<COST>
	{
	public:
		virtual void Execute() = 0;
		virtual const shared_ptr<CheckableFact<COST>> GetFact() const = 0;
	};

	template<typename T, typename COST = double>
	class ConclusionSettingFact : public Conclusion<COST>
	{
	public:
		ConclusionSettingFact(std::shared_ptr<FactSettable<T>> factToSet, const T& valueToBeSet, COST cost = 0) : factToBeSet(factToSet), valueToBeSet(std::make_unique<T>(valueToBeSet)), cost(cost){};
		using FactValue = T;
		virtual void Execute() override
		{
			factToBeSet->SetValue(*valueToBeSet);
		}


		virtual const shared_ptr<CheckableFact<COST>> GetFact() const override
		{
			return (shared_ptr<FactSettable<T>>)factToBeSet;
		}


		virtual const COST GetEstimatedCost() const override
		{
			return cost + factToBeSet->GetEstimatedCost();
		}

	protected:
		std::shared_ptr<FactSettable<FactValue>> factToBeSet;
		const std::unique_ptr<FactValue> valueToBeSet;
		COST cost;
	};

	template<std::invocable T, typename COST = double>
	class ConclusionInvokingFunction : public Conclusion<COST>
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
		virtual vector<shared_ptr<CheckableFact<COST>>> GetFactsForFiring() = 0;
		virtual vector<shared_ptr<CheckableFact<COST>>> GetFactsConcluding() = 0;
	};

	template<typename COST = double>
	class RuleImpl : public Rule<COST>
	{
	public:
		using CostType = COST;
		
		RuleImpl(std::vector<std::shared_ptr<Premise<CostType>>> premises, std::vector<std::shared_ptr<Conclusion<CostType>>> conclusions, CostType cost = 0) : premises(premises), conclusions(conclusions), cost(cost) {};
		RuleImpl(std::shared_ptr<Premise<CostType>> premise, std::vector<std::shared_ptr<Conclusion<CostType>>> conclusions, CostType cost = 0) : premises(std::vector<std::shared_ptr<Premise>>{premise}), conclusions(conclusions), cost(cost) {};
		RuleImpl(std::shared_ptr<Premise<CostType>> premise, std::shared_ptr<Conclusion<CostType>> conclusion, CostType cost = 0)
			: premises(std::vector<std::shared_ptr<Premise<CostType>>>{premise}),
			conclusions(std::vector<std::shared_ptr<Conclusion<CostType>>>{conclusion}), cost(cost) {};


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
				for (std::shared_ptr<Conclusion<CostType>> conclusion : conclusions)
				{
					conclusion->Execute();
				}
			} 
			return allSatisfied;
		}


		virtual const CostType GetEstimatedCost() const override
		{
			CostType sumCost = cost;
			for each (auto premise in premises)
			{
				sumCost += premise->GetEstimatedCost();
			}
			for each (auto conclusion in conclusions)
			{
				sumCost += conclusion->GetEstimatedCost();
			}

			return sumCost;
		}


		virtual vector<shared_ptr<CheckableFact<COST>>> GetFactsForFiring() override
		{
			vector<shared_ptr<CheckableFact<COST>>> toRet;
			for each (std::shared_ptr<Premise<CostType>> premise in premises)
			{
				toRet.push_back(premise->GetFact());
			}
			return toRet;
		}


		virtual vector<shared_ptr<CheckableFact<COST>>> GetFactsConcluding() override
		{
			vector<shared_ptr<CheckableFact<COST>>> toRet;
			for each (std::shared_ptr<Conclusion<CostType>> conclusion in conclusions)
			{
				toRet.push_back(conclusion->GetFact());
			}
			return toRet;
		}

	protected:
		std::vector<std::shared_ptr<Premise<CostType>>> premises;
		std::vector<std::shared_ptr<Conclusion<CostType>>> conclusions;
		const COST cost = 0;
	};
}
#endif // universal_reasoner_rule_h__

