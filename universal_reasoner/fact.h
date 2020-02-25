#ifndef universal_reasoner_fact_h__
#define universal_reasoner_fact_h__
#include <memory>

#include <concepts>
#include <stdexcept>

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
	template<typename VALUE>
	class FactWithGet
	{
	public:
		typedef VALUE ValueType;

		virtual const ValueType GetValue() const = 0;
		virtual std::shared_ptr<ValueType> GetValueShared() = 0;
	};

	template<typename VALUE>
	class FactWithSet
	{
	public:
		typedef VALUE ValueType;

		virtual void SetValue(const ValueType&) = 0;
	};

	template<typename VALUE>
	class Fact : public FactWithGet<VALUE>, FactWithSet<VALUE>
	{
	public:
		typedef VALUE ValueType;

		virtual const ValueType GetValue() const = 0;
		virtual std::shared_ptr<ValueType> GetValueShared() = 0;
		virtual void SetValue(const ValueType&) = 0;
		virtual bool IsSettable() = 0;
	};

	template<typename VALUE>
	class FactConst : public Fact<VALUE>
	{
	public:
		using ValueType = VALUE;

		FactConst(std::shared_ptr<ValueType> factValue) : factValue(factValue) {};
		FactConst(const ValueType& factValue) : factValue(std::make_shared<ValueType>(factValue))	{};

		FactConst() {};

		virtual const ValueType GetValue() const override {return *factValue;}
		virtual  std::shared_ptr<ValueType> GetValueShared() override {
			return factValue;
		};
		virtual void SetValue(const ValueType&) override {throw std::logic_error("Value of the fact is already set.");}
		virtual bool IsSettable() override { return false; }

	protected:
		std::shared_ptr<ValueType> factValue;
	};


	template<typename VALUE>
	class FactSettable : public Fact<VALUE>
	{
	public:
		using ValueType = VALUE;

		virtual const ValueType GetValue() const override;
		virtual void SetValue(const VALUE& valueToSet) override;
		virtual std::shared_ptr<ValueType> GetValueShared() override;
		virtual bool IsSettable() override { return settable; }

	protected:
		bool IsStillSettable() { return settable; };
		void SetValueIfValid(const VALUE& value) { factValue = std::make_shared< ValueType>(value); }
		bool settable = true;
		std::shared_ptr<ValueType> factValue;
	};


	template<typename VALUE, template <typename> typename FACT_CONST = FactConst, template <typename> typename FACT_SETTABLE = FactSettable>
	class FactWithValue;

	template<std::equality_comparable VALUE, template <typename> typename FACT_CONST, template <typename> typename FACT_SETTABLE>
	class FactWithValue<VALUE, FACT_CONST, FACT_SETTABLE>
	{
	public:
		using ValueType = VALUE;
		using FactConstType = FACT_CONST<VALUE>;
		using FactSettableType = FACT_SETTABLE<VALUE>;

		FactWithValue(std::shared_ptr<ValueType> factValue)
			: factValue(std::make_shared< FactConstType>(factValue)) {};
		FactWithValue(const ValueType& factValue)
			: factValue(std::make_shared< FactConstType>(factValue)) {};
		FactWithValue() : factValue(std::make_shared<FactSettableType>())
		{};
		FactWithValue(shared_ptr<FactSettableType> fact) : factValue(fact)
		{};
		const ValueType GetValue() const { return factValue->GetValue(); };
		std::shared_ptr<Fact<ValueType>> GetValueShared() { return factValue; };
		void SetValue(const ValueType& val)
		{
			factValue->SetValue(val);
		}

		bool isEqual(const std::shared_ptr<const FactWithValue<VALUE>> second)
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

	template<typename VALUE>
	void ureasoner::FactSettable<VALUE>::SetValue(const VALUE& valueToSet)
	{
		if (IsStillSettable())
		{
			SetValueIfValid(valueToSet);
			settable = false;
		}
	}

	template<typename VALUE>
	const VALUE ureasoner::FactSettable<VALUE>::GetValue() const
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

	template<typename VALUE>
	std::shared_ptr<VALUE> ureasoner::FactSettable<VALUE>::GetValueShared()
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
