#ifndef universal_reasoner_fact_h__
#define universal_reasoner_fact_h__
#include <memory>

#include <concepts>


// tested feature - to be used to choose Fact with and without operator<() member
template<typename T>
concept has_type_member = requires { typename T::type; };

namespace ureasoner
{

	// This concept tests whether 'T::type' is a valid type

// 
// 	template<typename T> requires std::totally_ordered<T> // or right after a template parameter list
// 		T add(T a, T b) { return a + b; }

	class Fact
	{
	public:
		virtual bool operator==(const std::shared_ptr<const Fact> second) = 0;
		virtual bool operator!=(const std::shared_ptr<const Fact> second)
		{
			return !this->operator==(second);
		}
	};

	class FactLG : public Fact
	{
		virtual bool operator<(const std::shared_ptr<const FactLG> second) = 0;
		virtual bool operator<=(const std::shared_ptr<const FactLG> second)
		{
			return (this->operator==(second)) or (this->operator<(second));
		}
		virtual bool operator>(const std::shared_ptr<const FactLG> second)
		{
			return !(this->operator==(second)) or (this->operator<(second));
		}
		virtual bool operator>=(const std::shared_ptr<const FactLG> second)
		{
			return (this->operator==(second)) or (this->operator>(second));
		}
	};

	template<typename VALUE>
	class FactWithValue : public Fact
	{
	public:
		typedef VALUE ValueType;
		FactWithValue(const ValueType factValue) : factValue(factValue) {};
		const ValueType GetValue() const {};
	protected:
		const ValueType factValue;
	};
}

#endif // universal_reasoner_fact_h__
