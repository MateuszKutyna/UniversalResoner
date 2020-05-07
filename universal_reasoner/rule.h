#ifndef universal_reasoner_rule_h__
#define universal_reasoner_rule_h__

#include <memory>
#include <functional>
#include <vector>
#include "executableWithCost.h"
#include "fact.h"

namespace ureasoner
{
	template<typename COST = double>
	class Premise : public ExecutableWithCost<COST>
	{
	public:
		virtual bool Evaluate() const = 0;
		virtual bool Evaluate() = 0;
		virtual const std::shared_ptr<CheckableFact<COST>> GetFact() const = 0;
	};

	template<typename T, template<typename> typename FACT_TYPE = FactWithGet, typename COST = double>
	class PremiseWithType : public Premise<COST>
	{
	public:
		template<typename FACT_VALUE_TYPE>
		using FactType = FACT_TYPE<FACT_VALUE_TYPE>;
		using LocalFact = FactType<T>;
		using FactValueType = T;

		PremiseWithType(const std::shared_ptr<LocalFact> compareLeft, const FactValueType& compareRight, const COST& cost = 0,
			bool (*constComparer)(const FactValueType&, const FactValueType&) = [](const FactValueType& x, const FactValueType& y)->bool { return x == y; },
			bool (*comparer)(FactValueType&, FactValueType&) = [](FactValueType& x, FactValueType& y)->bool { return x == y; })
			: compareLeft(compareLeft), compareRight(std::make_unique<FactConst<T>>(compareRight)), comparer(comparer), constComparer(constComparer)/*, cost(cost)*/ {
			Premise::ExecutableWithCost::SetCost(cost);
		};

		virtual bool Evaluate() const override { return constComparer(compareLeft->GetValue(), compareRight->GetValue()); }
		virtual bool Evaluate() override { return comparer(*compareLeft->GetValueShared(), *compareRight->GetValueShared()); }
		virtual const std::shared_ptr<CheckableFact<COST>> GetFact() const override { return compareLeft; }
		virtual const COST GetEstimatedCost() const override { return Premise::ExecutableWithCost::GetCost() + compareLeft->GetEstimatedCost() + compareRight->GetEstimatedCost(); }

	protected:
		const std::shared_ptr<LocalFact> compareLeft;
		const std::unique_ptr<LocalFact> compareRight;
		bool (*comparer)(FactValueType&, FactValueType&);
		bool (*constComparer)(const FactValueType&, const FactValueType&);
	};

	template< typename COST = double>
	class Conclusion : public ExecutableWithCost<COST>
	{
	public:
		virtual void Execute() = 0;
		virtual const std::shared_ptr<CheckableFact<COST>> GetFact() const = 0;
	};

	template<typename T, typename COST = double>
	class ConclusionSettingFact : public Conclusion<COST>
	{
	public:
		using FactValue = T;
		ConclusionSettingFact(std::shared_ptr<FactSettable<T>> factToSet, const T& valueToBeSet, COST cost = 0) : factToBeSet(factToSet), valueToBeSet(std::make_unique<T>(valueToBeSet)) {
			Conclusion::ExecutableWithCost::SetCost(cost);
		};

		virtual void Execute() override												{factToBeSet->SetValue(*valueToBeSet);}
		virtual const std::shared_ptr<CheckableFact<COST>> GetFact() const override	{return factToBeSet;}
		virtual const COST GetEstimatedCost() const override						{return Conclusion::ExecutableWithCost::GetCost() + factToBeSet->GetEstimatedCost();}

	protected:
		std::shared_ptr<FactSettable<FactValue>> factToBeSet;
		const std::unique_ptr<FactValue> valueToBeSet;
	};

// NOT USED
// 	template<std::invocable T, typename COST = double>
// 	class ConclusionInvokingFunction : public Conclusion<COST>
// 	{
// 	public:
// 		using FactValue = T;
// 		virtual void Execute() override
// 		{
// 			toInvoke();
// 		}
// 
// 	protected:
// 		const T toInvoke();
// 	};

	template<typename COST>
	class Rule :
		public ExecutableWithCost<COST>
	{
	public:
		virtual bool CheckAndFire() = 0;
		virtual std::vector<std::shared_ptr<CheckableFact<COST>>> GetFactsForFiring() = 0;
		virtual std::vector<std::shared_ptr<CheckableFact<COST>>> GetFactsConcluding() = 0;
	};

	template<typename COST = double>
	class RuleImpl : public Rule<COST>
	{
	public:
		using CostType = COST;

		RuleImpl(std::vector<std::shared_ptr<Premise<CostType>>> premises, std::vector<std::shared_ptr<Conclusion<CostType>>> conclusions, CostType cost = 0) : premises(premises), conclusions(conclusions) {
			Rule::ExecutableWithCost::SetCost(cost);
		};
		RuleImpl(std::shared_ptr<Premise<CostType>> premise, std::vector<std::shared_ptr<Conclusion<CostType>>> conclusions, CostType cost = 0) : premises(std::vector<std::shared_ptr<Premise>>{premise}), conclusions(conclusions) { Rule::ExecutableWithCost::SetCost(cost); };
		RuleImpl(std::shared_ptr<Premise<CostType>> premise, std::shared_ptr<Conclusion<CostType>> conclusion, CostType cost = 0)
			: premises(std::vector<std::shared_ptr<Premise<CostType>>>{premise}),
			conclusions(std::vector<std::shared_ptr<Conclusion<CostType>>>{conclusion}) {
			Rule::ExecutableWithCost::SetCost(cost);
		};

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
			CostType sumCost = Rule::ExecutableWithCost::GetCost();
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

		virtual std::vector<std::shared_ptr<CheckableFact<COST>>> GetFactsForFiring() override
		{
			std::vector<std::shared_ptr<CheckableFact<COST>>> toRet;
			for each (std::shared_ptr<Premise<CostType>> premise in premises)
			{
				toRet.push_back(premise->GetFact());
			}
			return toRet;
		}

		virtual std::vector<std::shared_ptr<CheckableFact<COST>>> GetFactsConcluding() override
		{
			std::vector<std::shared_ptr<CheckableFact<COST>>> toRet;
			for each (std::shared_ptr<Conclusion<CostType>> conclusion in conclusions)
			{
				toRet.push_back(conclusion->GetFact());
			}
			return toRet;
		}

	protected:
		std::vector<std::shared_ptr<Premise<CostType>>> premises;
		std::vector<std::shared_ptr<Conclusion<CostType>>> conclusions;
	};
}
#endif // universal_reasoner_rule_h__

