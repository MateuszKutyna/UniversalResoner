#ifndef universal_reasoner_rule_h__
#define universal_reasoner_rule_h__
#include<iostream>
#include <memory>
#include <functional>
#include <vector>
#include "executableWithCost.h"
#include "fact.h"
#include<ppl.h>
#include<concurrent_vector.h>
#include <thread>
#include <random>
#include <chrono>

namespace ureasoner
{
	template<typename COST = double>
	class Premise : public ExecutableWithCost<COST>{
	public:
		virtual bool Evaluate() const = 0;
		virtual bool Evaluate() = 0;
		virtual std::shared_ptr<CheckableFact<COST>> GetFact() const = 0;
	};

	template<typename T, template<typename> typename FACT_TYPE = FactWithGet, typename COST = double>
	class PremiseWithType : public Premise<COST>{
	public:
		template<typename FACT_VALUE_TYPE>
		using FactType = FACT_TYPE<FACT_VALUE_TYPE>;
		using LocalFact = FactType<T>;
		using FactValueType = T;
		using ComparerType = std::function<bool(const FactValueType&, const FactValueType&)>;

		//Bierze znany fakt i sprawdza czy jest rowny premisowi
		PremiseWithType(const std::shared_ptr<LocalFact> compareLeft, const FactValueType& compareRight, const COST& cost = 0,
			ComparerType constComparer = [](const FactValueType& x, const FactValueType& y) { return x == y; })
			: compareLeft(compareLeft), compareRight(std::make_unique<FactConst<T>>(compareRight)),
			constComparer(constComparer){
			Premise<COST>::ExecutableWithCost::SetCost(cost);
		};

		virtual bool Evaluate() const override { return constComparer(compareLeft->GetValue(), compareRight->GetValue()); }
		virtual bool Evaluate() override { return constComparer(*compareLeft->GetValueShared(), *compareRight->GetValueShared()); }
		virtual std::shared_ptr<CheckableFact<COST>> GetFact() const override { return compareLeft; }
		virtual COST GetEstimatedCost() const override { return Premise<COST>::ExecutableWithCost::GetCost() + compareLeft->GetEstimatedCost() + compareRight->GetEstimatedCost(); }
	protected:
		const std::shared_ptr<LocalFact> compareLeft;
		const std::unique_ptr<LocalFact> compareRight;
		ComparerType constComparer;
	};

	template< typename COST = double>
	class Conclusion : public ExecutableWithCost<COST>
	{
	public:
		virtual void Execute() = 0;
		virtual std::shared_ptr<CheckableFact<COST>> GetFact() const = 0;
	};

	template<typename T, typename COST = double>
	class ConclusionSettingFact : public Conclusion<COST>
	{
	public:
		using FactValue = T;
		ConclusionSettingFact(std::shared_ptr<FactSettable<T>> factToSet, const T& valueToBeSet, COST cost = 0) : factToBeSet(factToSet), valueToBeSet(std::make_unique<T>(valueToBeSet)) {
			Conclusion<COST>::ExecutableWithCost::SetCost(cost);
		};

		virtual void Execute() override												{factToBeSet->SetValue(*valueToBeSet);}
		virtual std::shared_ptr<CheckableFact<COST>> GetFact() const override		{return factToBeSet;}
		virtual COST GetEstimatedCost() const override								{return Conclusion<COST>::ExecutableWithCost::GetCost() + factToBeSet->GetEstimatedCost();}

	protected:
		std::shared_ptr<FactSettable<FactValue>> factToBeSet;
		const std::unique_ptr<FactValue> valueToBeSet;
	};


	template<typename COST>
	class Rule : public ExecutableWithCost<COST>
	{
	public:
		virtual bool CheckAndFire() = 0;
		virtual std::vector<std::shared_ptr<CheckableFact<COST>>> GetFactsForFiring() = 0;
		virtual std::vector<std::shared_ptr<CheckableFact<COST>>> GetFactsConcluding() = 0;
	};

	template<typename COST = double>
	class RuleImpl : public Rule<COST> //RuleImplementaion?
	{
	public:
		using CostType = COST;

		RuleImpl(Concurrency::concurrent_vector<std::shared_ptr<Premise<CostType>>> premises, Concurrency::concurrent_vector<std::shared_ptr<Conclusion<CostType>>> conclusions, CostType cost = 0) : premises(premises), conclusions(conclusions) {
			Rule<COST>::ExecutableWithCost::SetCost(cost);
		};
		RuleImpl(std::shared_ptr<Premise<CostType>> premise, Concurrency::concurrent_vector<std::shared_ptr<Conclusion<CostType>>> conclusions, CostType cost = 0) : premises(Concurrency::concurrent_vector<std::shared_ptr<Premise>>{premise}), conclusions(conclusions) { Rule::ExecutableWithCost::SetCost(cost); };
		RuleImpl(std::shared_ptr<Premise<CostType>> premise, std::shared_ptr<Conclusion<CostType>> conclusion, CostType cost = 0)
			: premises(std::vector<std::shared_ptr<Premise<CostType>>>{premise}),
			conclusions(std::vector<std::shared_ptr<Conclusion<CostType>>>{conclusion}) {
			Rule<CostType>::ExecutableWithCost::SetCost(cost);
		};

		virtual bool CheckAndFire() override
		{
			bool allSatisfied = true;
			auto iter = premises.begin();

			std::mt19937_64 eng{ std::random_device{}() };  
			std::uniform_int_distribution<> dist( 10, 1000 );
			std::this_thread::sleep_for(std::chrono::milliseconds{ dist(eng) });
			
			//Sprawdza czy premisy zgadzaja sie z faktem
			while (allSatisfied && (iter != premises.end()))
			{
				allSatisfied = (*iter++)->Evaluate();
			}
			//Jezeli sie zgadza to ustawia konkluzje jako nowy fakt
			if (allSatisfied)
			{
				Concurrency::parallel_for_each(std::begin(conclusions), std::end(conclusions), [&](auto conclusion) {
					conclusion->Execute();
					});

			}
			return allSatisfied;
		}

		virtual CostType GetEstimatedCost() const override
		{
			CostType sumCost = Rule<CostType>::ExecutableWithCost::GetCost();
			Concurrency::parallel_for_each(std::begin(premises), std::end(premises), [&](auto premise) {
				sumCost += premise->GetEstimatedCost();
			});

			Concurrency::parallel_for_each(std::begin(conclusions), std::end(conclusions), [&](auto conclusion) {
				sumCost += conclusion->GetEstimatedCost();
			});
			
			return sumCost;
		}

		virtual std::vector<std::shared_ptr<CheckableFact<COST>>> GetFactsForFiring() override
		{
			std::vector<std::shared_ptr<CheckableFact<COST>>> toRet;
			Concurrency::parallel_for_each(std::begin(premises), std::end(premises), [&](auto premise) {
				toRet.push_back(premise->GetFact());
			});
			return toRet;
		}

		virtual std::vector<std::shared_ptr<CheckableFact<COST>>> GetFactsConcluding() override
		{
			std::vector<std::shared_ptr<CheckableFact<COST>>> toRet;
			Concurrency::parallel_for_each(std::begin(conclusions), std::end(conclusions), [&](auto conclusion) {
				toRet.push_back(conclusion->GetFact());
				});
			return toRet;
		}

	protected:
		Concurrency::concurrent_vector<std::shared_ptr<Premise<CostType>>> premises;
		Concurrency::concurrent_vector<std::shared_ptr<Conclusion<CostType>>> conclusions;
	};
}
#endif // universal_reasoner_rule_h__

