#ifndef universal_reasoner_fact_h__
#define universal_reasoner_fact_h__
#include <memory>

#include <concepts>
#include <stdexcept>
#include "executableWithCost.h"

using std::shared_ptr;
using std::make_shared;
// tested feature - to be used to choose Fact with and without operator<() member
// template<typename T>
// concept has_type_member = requires {totally_ordered <T>; };
// 
// class T1
// {
// public:
// 	int i;
// };
// 
// inline bool operator==(const T1& lhs, const T1& rhs) { return (lhs.i == rhs.i); }
// inline bool operator!=(const T1& lhs, const T1& rhs) { return !(lhs == rhs); }
// 
// template<class T>
// struct test1
// {
// 	int secVal;
// 	static const int val = 0;
// };
// 
// template<class T>
// struct wrapper {
// 	static const int val = 0;
// };;
// 
// template<std::totally_ordered T>
// struct wrapper<T>
// {
// 	static const int val = 1;
// };
// 
// template<std::equality_comparable T>
// struct wrapper<T>
// {
// 	static const int val = 2;
//  };
// 
//  


namespace ureasoner
{
	template<typename COST = double>
	class CheckableFact: public ExecutableWithCost<COST>
	{
	public:
		virtual bool IsKnown() const = 0;
	};

	template<typename VALUE, typename COST = double>
	class FactWithGet: virtual public CheckableFact<COST>
	{
	public:
		typedef VALUE ValueType;

		virtual const ValueType GetValue() const = 0;
		virtual std::shared_ptr<ValueType> GetValueShared() = 0;	
		virtual const COST GetEstimatedGetCost() const = 0;
		
	};

	template<typename VALUE, typename COST = double>
	class FactWithSet: virtual public CheckableFact<COST>
	{
	public:
		typedef VALUE ValueType;

		virtual void SetValue(const ValueType&) = 0;
		virtual const COST GetEstimatedSetCost() const = 0;

	};

	template<typename VALUE, typename COST = double>
	class Fact : public FactWithGet<VALUE, COST>, public FactWithSet<VALUE, COST>
	{
	public:
		typedef VALUE ValueType;
		virtual bool IsSettable() const = 0;
	};

	template<typename VALUE, typename COST = double>
	class FactConst : public Fact<VALUE, COST>
	{
	public:
		using ValueType = VALUE;

		FactConst(std::shared_ptr<ValueType> factValue, const COST& cost = 0) : factValue(factValue), cost(cost) {};
		FactConst(const ValueType& factValue, const COST& cost = 0) : factValue(std::make_shared<ValueType>(factValue)), cost(cost) {};

		//FactConst() {};

		virtual const ValueType GetValue() const override {
			return *factValue;
		}
		virtual  std::shared_ptr<ValueType> GetValueShared() override {
			return std::shared_ptr<ValueType>(factValue);
		};
		virtual void SetValue(const ValueType&) override {throw std::logic_error("Value of the fact is already set.");}
		virtual bool IsSettable() const override { return false; }
		virtual bool IsKnown() const override { return true; };


		virtual const COST GetEstimatedCost() const override
		{
			return GetEstimatedGetCost() + GetEstimatedSetCost();
		}

	protected:
		virtual const COST GetEstimatedGetCost() const override
		{
			return  cost;
		}
		virtual const COST GetEstimatedSetCost() const override
		{
			return 0;
		}


		std::shared_ptr<ValueType> factValue;
		const COST cost;
	};


	template<typename VALUE, typename COST = double>
	class FactSettable : public Fact<VALUE, COST>
	{
	public:
		using ValueType = VALUE;

		FactSettable(const COST& costGet = 0, const COST& costSet = 0) : costGet(costGet), costSet(costSet){};
		virtual const ValueType GetValue() const override
		{
			if (settable)
			{
				throw std::logic_error("Value of the fact is not set.");
			}
			else
			{
				return *factValue;
			}
		}
		virtual void SetValue(const VALUE& valueToSet) override
		{
			if (IsStillSettable())
			{
				SetValueIfValid(valueToSet);
				settable = false;
			}
		}
		virtual std::shared_ptr<ValueType> GetValueShared() override;
		virtual bool IsSettable() const override { return settable; }
		virtual bool IsKnown() const override { return !IsSettable(); };



		virtual const COST GetEstimatedCost() const override
		{
			return GetEstimatedGetCost() + GetEstimatedSetCost();
		}


// 		virtual void SetValue(const ValueType&) override
// 		{
// 			throw std::logic_error("The method or operation is not implemented.");
// 		}

	protected:
		bool IsStillSettable() { return settable; };
		void SetValueIfValid(const VALUE& value) { factValue = std::make_shared< ValueType>(value); }
		bool settable = true;
		std::shared_ptr<ValueType> factValue;

		const COST costGet;
		const COST costSet;

		virtual const COST GetEstimatedGetCost() const override
		{
			return costGet;
		}
		virtual const COST GetEstimatedSetCost() const override
		{
			return costSet;
		}
	};


	template<typename VALUE, template <typename> typename FACT_CONST = FactConst, template <typename> typename FACT_SETTABLE = FactSettable>
	class FactRepresentation;

	template<std::equality_comparable VALUE, template <typename> typename FACT_CONST, template <typename> typename FACT_SETTABLE>
	class FactRepresentation<VALUE, FACT_CONST, FACT_SETTABLE>
	{
	public:
		using ValueType = VALUE;
		using FactConstType = FACT_CONST<VALUE>;
		using FactSettableType = FACT_SETTABLE<VALUE>;

		FactRepresentation(std::shared_ptr<ValueType> factValue)
			: factValue(std::make_shared< FactConstType>(factValue)) {};
		FactRepresentation(const ValueType& factValue)
			: factValue(std::make_shared< FactConstType>(factValue)) {};
		FactRepresentation() : factValue(std::make_shared<FactSettableType>())
		{};
		FactRepresentation(shared_ptr<FactSettableType> fact) : factValue(fact)
		{};
		const ValueType GetValue() const { return factValue->GetValue(); };
		std::shared_ptr<Fact<ValueType>> GetValueShared() { return factValue; };
		void SetValue(const ValueType& val)
		{
			factValue->SetValue(val);
		}

		bool isEqual(const std::shared_ptr<const FactRepresentation<VALUE>> second)
		{
			return(GetValue() == second->GetValue());
		};
		bool IsSettable()
		{
			return factValue->IsSettable();
		}

	protected:
		std::shared_ptr<Fact<ValueType>> factValue;
	};

// Implementation ///////////////////////////////////////////////////////////////////////////////////////////
// 
// 	template<typename VALUE, typename COST>
// 	void ureasoner::FactSettable<VALUE>::SetValue(const VALUE& valueToSet)
// 	{
// 		if (IsStillSettable())
// 		{
// 			SetValueIfValid(valueToSet);
// 			settable = false;
// 		}
// 	}

// 	template<typename VALUE, typename COST>
// 	const VALUE ureasoner::FactSettable<VALUE>::GetValue() const
// 	{
// 		if (settable)
// 		{
// 			throw std::logic_error("Value of the fact is not set.");
// 		}
// 		else
// 		{
// 			return *factValue;
// 		}
// 	}

	template<typename VALUE, typename COST>
	std::shared_ptr<VALUE> ureasoner::FactSettable<VALUE, COST>::GetValueShared()
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
}

#endif // universal_reasoner_fact_h__
