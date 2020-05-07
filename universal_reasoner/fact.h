#ifndef universal_reasoner_fact_h__
#define universal_reasoner_fact_h__

#include <memory>
#include <stdexcept>
#include "executableWithCost.h"
#include "../knowledge_importers/knowledge_importers.h"

namespace ureasoner
{
	template<totally_ordered COST = double>
	class CheckableFact: public ExecutableWithCost<COST>
	{
	public:
		virtual ~CheckableFact() noexcept{};
		virtual bool IsKnown() const noexcept = 0;
	};

	template<typename VALUE, typename COST = double>
	class FactWithGet: public CheckableFact<COST>
	{
	public:
		using ValueType = VALUE;

		virtual ~FactWithGet() noexcept {}; 

		virtual const ValueType GetValue() const = 0;
		virtual std::shared_ptr<ValueType> GetValueShared() = 0;	
		virtual const COST GetEstimatedGetCost() const = 0;	
	};


	template<typename VALUE, typename COST = double>
	class Fact : public FactWithGet<VALUE, COST>
	{
	public:
		using ValueType = FactWithGet<VALUE, COST>::ValueType;

		virtual ~Fact() noexcept {}

		virtual bool IsSettable() const noexcept = 0;
		virtual void SetValue(const ValueType&) = 0;
		virtual const COST GetEstimatedSetCost() const = 0;

	};

	template<typename VALUE, typename COST = double>
	class FactConst : public Fact<VALUE, COST>
	{
	public:
		using CostType = COST;
		using ValueType = Fact<VALUE, COST>::ValueType;

		FactConst(const std::shared_ptr<ValueType> factValue, const COST& cost = 0) : factValue(factValue), cost(cost) {};
		FactConst(const ValueType& factValue, const COST& cost = 0) : factValue(std::make_shared<ValueType>(factValue)) {this->cost = cost; };
		virtual ~FactConst() {};

		virtual const ValueType GetValue() const override					{return *factValue;}
		virtual  std::shared_ptr<ValueType> GetValueShared() override		{return std::shared_ptr<ValueType>(factValue);};
 		virtual void SetValue(const ValueType&) override					{throw std::logic_error("Value of the fact is already set.");}
 		constexpr virtual bool IsSettable() const noexcept override			{return false; }
		constexpr virtual bool IsKnown() const noexcept override			{return true; };
		virtual const COST GetEstimatedCost() const override				{return GetEstimatedGetCost();}

	protected:
		virtual const COST GetEstimatedGetCost() const override				{return Fact::FactWithGet::CheckableFact::ExecutableWithCost::GetCost();}
		constexpr virtual const COST GetEstimatedSetCost() const override	{return 0;}

		std::shared_ptr<ValueType> factValue;
		CostType cost;
	};

	template<typename VALUE, typename COST = double>
	class FactSettable : public Fact<VALUE, COST>
	{
	public:
		using CostType = COST;
		using ValueType = Fact<VALUE, COST>::ValueType;

		FactSettable(const COST& cost = 0, const COST& costGet = 0, const COST& costSet = 0) : costGet(costGet), costSet(costSet) 
		{
			Fact::CheckableFact::ExecutableWithCost::SetCost(cost);
		};
		FactSettable(const importer::EmptyVar<ValueType>&) : costGet(0), costSet(0)	{};
		template <typename T>
		FactSettable(const importer::EmptyVar<T>&) : costGet(0), costSet(0)		{throw std::logic_error("Wrong type conversion during input");};
		virtual ~FactSettable() {};

		virtual const ValueType GetValue() const override;
		virtual void SetValue(const VALUE& valueToSet) override;
		virtual std::shared_ptr<ValueType> GetValueShared() override;
		virtual bool IsSettable() const noexcept override				{return settable; }
		virtual bool IsKnown() const noexcept override					{return !IsSettable(); };
		virtual const COST GetEstimatedCost() const override			{return Fact::CheckableFact::ExecutableWithCost::GetCost() + GetEstimatedGetCost() + GetEstimatedSetCost();}

	protected:
		bool settable = true;
		std::shared_ptr<ValueType> factValue;
		const COST costGet;
		const COST costSet;

		virtual const COST GetEstimatedGetCost() const override			{return costGet;}
		virtual const COST GetEstimatedSetCost() const override			{return costSet;}
	};

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPLEMENTATION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	template<typename VALUE, typename COST>
	std::shared_ptr<VALUE> FactSettable<VALUE, COST>::GetValueShared()
	{
		if (settable)
		{
			throw std::logic_error("Value of the fact is not set.");
		}
		else
		{
			return factValue;
		}
	}

	template<typename VALUE, typename COST>
	const VALUE FactSettable<VALUE, COST>::GetValue() const
	{
		if (IsSettable())
		{
			throw std::logic_error("Value of the fact is not set.");
		}
		else
		{
			return *factValue;
		}
	}
	template<typename VALUE, typename COST>
	void FactSettable<VALUE, COST>::SetValue(const VALUE& valueToSet)
	{
		if (IsSettable())
		{
			factValue = std::make_shared<ValueType>(valueToSet);
			settable = false;
		}
	}
}

#endif // universal_reasoner_fact_h__
