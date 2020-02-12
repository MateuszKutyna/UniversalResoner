#ifndef universal_reasoner_fact_h__
#define universal_reasoner_fact_h__
#include <memory>

#include <concepts>


// tested feature - to be used to choose Fact with and without operator<() member
template<typename T>
concept has_type_member = requires {totally_ordered <T>; };

class T1
{
public:
	int i;
};

inline bool operator==(const T1& lhs, const T1& rhs) { return (lhs.i == rhs.i); }
inline bool operator!=(const T1& lhs, const T1& rhs) { return !(lhs == rhs); }

template<class T>
struct test1
{
	int secVal;
	static const int val = 0;
};

template<class T>
struct wrapper {
	static const int val = 0;
};;

template<std::totally_ordered T>
struct wrapper<T>
{
	static const int val = 1;
};

template<std::equality_comparable T>
struct wrapper<T>
{
	static const int val = 2;
 };

 


namespace ureasoner
{
	class Fact
	{
	public:
// 		virtual bool isEqual(const std::shared_ptr<const Fact> second) = 0;
// 		virtual bool isNotEqual(const std::shared_ptr<const Fact> second)
// 		{
// 			return !this->isEqual(second);
// 		}
	};

	class FactLG : public Fact
	{
// 		virtual bool isLt(const std::shared_ptr<const FactLG> second) = 0;
// 		virtual bool isLeq(const std::shared_ptr<const FactLG> second)
// 		{
// 			return (this->isEqual(second)) || (this->isLt(second));
// 		}
// 		virtual bool isGt(const std::shared_ptr<const FactLG> second)
// 		{
// 			return !(this->isEqual(second)) || (this->isLt(second));
// 		}
// 		virtual bool operator>=(const std::shared_ptr<const FactLG> second)
// 		{
// 			return (this->isEqual(second)) || (this->isGt(second));
// 		}
	};

	template<typename VALUE>
	class FactWithValue;

// 	template<std::totally_ordered VALUE>
// 	class FactWithValue<VALUE> : public FactLG
// 	{
// 	public:
// 		typedef VALUE ValueType;
// 		FactWithValue(std::unique_ptr<ValueType> factValue) : factValue(std::move(factValue)) {};
// 		const ValueType GetValue() const {};
// 		virtual bool operator==(const std::shared_ptr<const Fact> second) override
// 		{
// 			return(*factValue == *second);
// 		};
// 		virtual bool operator<(const std::shared_ptr<const FactLG> second) override
// 		{
// 			return(false/*factValue < *second*/);
// 		};
// 	protected:
// 		const std::unique_ptr<ValueType> factValue;
// 	};

	template<std::equality_comparable VALUE>
	class FactWithValue<VALUE> : public Fact
	{
	public:
		typedef VALUE ValueType;
		FactWithValue(std::shared_ptr<ValueType> factValue) : factValue(factValue) {};
		FactWithValue(const ValueType& factValue) : factValue(std::make_shared<ValueType>(factValue)) {};
		const ValueType GetValue() const { return *factValue; };

		bool isEqual(const std::shared_ptr<const FactWithValue<VALUE>> second)
		{
			return(*factValue==second->GetValue());
		};
	protected:
		const std::shared_ptr<ValueType> factValue;
	};

	template<typename VALUE>
	class FactSettable
	{
	public:
		using ValueType = VALUE;
		void SetValue(const VALUE& valueToSet)
		{
			if (IsStillSettable())
			{
				SetValueIfValid(valueToSet);
			}
		};
	protected:
		virtual bool IsStillSettable() = 0;
		virtual void SetValueIfValid(const VALUE&) = 0;
	};
}

#endif // universal_reasoner_fact_h__
