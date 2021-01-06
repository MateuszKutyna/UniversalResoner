#ifndef universal_reasoner_factsRepository_h__
#define universal_reasoner_factsRepository_h__

#include "fact.h"
#include <unordered_map>
#include <type_traits>
#include<concurrent_unordered_map.h>
#include<concurrent_vector.h>
#include<ppl.h>
namespace ureasoner
{
	template <typename T, typename... List>
	struct IsContained;

	//check if types aren't duplicating
	template <typename T, typename Head, typename... Tail>
	struct IsContained<T, Head, Tail...>
	{
		enum { value = std::is_same<T, Head>::value || (IsContained<T, Tail...>::value) };
	};

	// if there is odd an odd number of types 
	template <typename T>
	struct IsContained<T>
	{
		enum { value = false };
	};

	template <typename COST, typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	class FactsRepository : public FactsRepository<COST, STORED_TYPES...>{
		static_assert(!IsContained<FIRST_STORED_TYPE, STORED_TYPES...>::value, "No duplicate types allowed");

	public:
		using FactsRepository<COST, STORED_TYPES...>::AddFact;
		using FactsRepository<COST, STORED_TYPES...>::GetFact;
		using StoredType = FIRST_STORED_TYPE;
		using CostType = COST;

		template<typename T>
		auto GetFactByName(const std::string& name);	// auto here allows to pass a result from inherited GetFact function!
		template<typename T>
		auto GetFactByNameDynamic(const std::string& name);	// auto here allows to pass a result from inherited GetFact function!
		template<typename T>
		auto GetSettableFactByName(const std::string& name);	// auto here allows to pass a result from inherited GetFact function!

		std::shared_ptr< Fact<StoredType>> AddFact(std::shared_ptr<Fact<StoredType>> fact, const std::string& name);
		std::shared_ptr< Fact<StoredType>> AddFact(const StoredType& fact, const std::string& name);
		std::shared_ptr< Fact<StoredType>> AddFact(const FactSettable<StoredType>& fact, const std::string& name)
		{
			return storage.insert(std::pair<std::string, std::shared_ptr< Fact<StoredType>>>(name, std::make_shared< FactSettable<StoredType>>(fact))).first->second;
		};
		std::shared_ptr<Concurrency::concurrent_vector<std::shared_ptr<CheckableFact<COST>>>> GetAllKnownFacts();

	protected:
		//Keys must be unique (in this case std::string), it has no order
		Concurrency::concurrent_unordered_map<std::string, std::shared_ptr<Fact<StoredType>>> storage;
		std::shared_ptr< Fact<StoredType>> GetFact(const std::string& name, const StoredType*);
		std::shared_ptr< FactSettable<StoredType, COST>> GetSettableFact(const std::string& name, const StoredType*);
	};

	template <typename COST, typename FIRST_STORED_TYPE>
	class FactsRepository< COST, FIRST_STORED_TYPE>{
	public:
		using StoredType = FIRST_STORED_TYPE;
		using CostType = COST;

		template<typename T>
		std::shared_ptr< Fact<StoredType>> GetFactByName(const std::string& name);
		template<typename T>
		std::shared_ptr< Fact<T>> GetFactByNameDynamic(const std::string& name);
		std::shared_ptr< Fact<StoredType>> GetSettableFactByName(const std::string& name);
		std::shared_ptr< Fact<StoredType>> AddFact(const std::shared_ptr<Fact<StoredType>>& fact, const std::string& name);
		std::shared_ptr< Fact<StoredType>> AddFact(const StoredType& fact, const std::string& name);
		std::shared_ptr< Fact<StoredType>> AddFact(const FactSettable<StoredType>& fact, const std::string& name)
		{
			return storage.insert(std::pair<std::string, std::shared_ptr< Fact<StoredType>>>(name, std::make_shared< FactSettable<StoredType>>(fact))).first->second;
		};
		std::shared_ptr<Concurrency::concurrent_vector<std::shared_ptr<CheckableFact<COST>>>> GetAllKnownFacts();

	protected:
		//Keys must be unique
		Concurrency::concurrent_unordered_map<std::string, std::shared_ptr<Fact<StoredType>>> storage;
		//std::unordered_map<std::string, std::shared_ptr< Fact<StoredType>>> storage;
		std::shared_ptr< Fact<StoredType>> GetFact(const std::string& name, const StoredType*);
		std::shared_ptr< FactSettable<StoredType, COST>> GetSettableFact(const std::string& name, const StoredType*);

	};


	template <typename... STORED_TYPES>
	using FactsRepositoryDouble = FactsRepository<double, STORED_TYPES...>;

	//????????????????????????????????????? IMPLEMENTATION //////////////////////////////////////////////////////////////////////////////////

	template <typename COST, typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	template<typename T>
	auto FactsRepository<COST, FIRST_STORED_TYPE, STORED_TYPES...>::GetFactByName(const std::string& name)
	{
		T* trait = nullptr;
		return GetFact(name, trait);
	}

	template <typename COST, typename FIRST_STORED_TYPE>
	template<typename T>
	std::shared_ptr<Fact<FIRST_STORED_TYPE>> FactsRepository<COST, FIRST_STORED_TYPE>::GetFactByName(const std::string& name)
	{
		static_assert(std::is_same<T, StoredType>::value, "FactRepository do not store this type");
		T* trait = nullptr;
		return GetFact(name, trait);
	};

	template <typename COST, typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	template<typename T>
	auto FactsRepository<COST, FIRST_STORED_TYPE, STORED_TYPES...>::GetFactByNameDynamic(const std::string& name)
	{
		return GetFactByName<T>(name);
	}

	template <typename COST, typename FIRST_STORED_TYPE>
	template<typename T>
	std::shared_ptr<Fact<T>> FactsRepository<COST, FIRST_STORED_TYPE>::GetFactByNameDynamic(const std::string& name)
	{
		if constexpr (std::is_same<T, FIRST_STORED_TYPE>::value)
			return GetFactByName<T>(name);
		else
			throw std::logic_error("Error during type conversion");
	};

	template <typename COST, typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	template<typename T>
	auto FactsRepository<COST, FIRST_STORED_TYPE, STORED_TYPES...>::GetSettableFactByName(const std::string& name)
	{
		T* trait = nullptr;
		return GetSettableFact(name, trait);
	}

	template <typename COST, typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	std::shared_ptr<Fact<FIRST_STORED_TYPE>> FactsRepository<COST, FIRST_STORED_TYPE, STORED_TYPES...>::AddFact(const FIRST_STORED_TYPE& fact, const std::string& name)
	{
		return AddFact(std::make_shared<FactConst<FIRST_STORED_TYPE>>(fact), name);
	}

	template <typename COST, typename FIRST_STORED_TYPE>
	std::shared_ptr<Fact<FIRST_STORED_TYPE>> FactsRepository<COST, FIRST_STORED_TYPE>::AddFact(const FIRST_STORED_TYPE& fact, const std::string& name)
	{
		return AddFact(std::make_shared<FactConst<FIRST_STORED_TYPE>>(fact), name);
	}

	template <typename COST, typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	std::shared_ptr<Fact<FIRST_STORED_TYPE>> FactsRepository<COST, FIRST_STORED_TYPE, STORED_TYPES...>::AddFact(std::shared_ptr<Fact<FIRST_STORED_TYPE>> fact, const std::string& name)
	{
		auto toRet = storage.insert(std::pair<std::string, std::shared_ptr< Fact<StoredType>>>(name, fact)).first->second;
		return toRet;
	}

	template <typename COST, typename FIRST_STORED_TYPE>
	std::shared_ptr<Fact<FIRST_STORED_TYPE>> FactsRepository<COST, FIRST_STORED_TYPE>::AddFact(const std::shared_ptr<Fact<FIRST_STORED_TYPE>>& fact, const std::string& name)
	{
		auto toRet = storage.insert(std::pair<std::string, std::shared_ptr< Fact<StoredType>>>(name, fact)).first->second;
		return toRet;
	}

	template <typename COST, typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	std::shared_ptr<Concurrency::concurrent_vector<std::shared_ptr<CheckableFact<COST>>>> FactsRepository<COST, FIRST_STORED_TYPE, STORED_TYPES...>::GetAllKnownFacts()
	{
		auto toRet = FactsRepository<COST, STORED_TYPES...>::GetAllKnownFacts();
		Concurrency::parallel_for_each(std::begin(storage), std::end(storage), [&](auto& fact) {
			if (fact.second->IsKnown()) {
				toRet->push_back(fact.second);
			}
			});
		return toRet;
	}

	template <typename COST, typename FIRST_STORED_TYPE>
	std::shared_ptr<Concurrency::concurrent_vector<std::shared_ptr<CheckableFact<COST>>>> FactsRepository<COST, FIRST_STORED_TYPE>::GetAllKnownFacts()
	{

		auto toRet = std::make_shared<Concurrency::concurrent_vector<std::shared_ptr<CheckableFact<COST>>>>();
		Concurrency::parallel_for_each(std::begin(storage), std::end(storage), [&](auto& fact) {
			if (fact.second->IsKnown()) {
				toRet->push_back(fact.second);
			}
		});

		return toRet;
	}

	template <typename COST, typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	std::shared_ptr<Fact<FIRST_STORED_TYPE>> FactsRepository<COST, FIRST_STORED_TYPE, STORED_TYPES...>::GetFact(const std::string& name, const FIRST_STORED_TYPE*)
	{
		return storage.at(name);
	}

	template <typename COST, typename FIRST_STORED_TYPE>
	std::shared_ptr<Fact<FIRST_STORED_TYPE>> FactsRepository<COST, FIRST_STORED_TYPE>::GetFact(const std::string& name, const StoredType*)
	{
		return storage.at(name);
	}

	template <typename COST, typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	std::shared_ptr<FactSettable<FIRST_STORED_TYPE, COST>> FactsRepository<COST, FIRST_STORED_TYPE, STORED_TYPES...>::GetSettableFact(const std::string& name, const FIRST_STORED_TYPE*)
	{
		auto toRet = storage.at(name);
		if (toRet->IsSettable())
		{
			return std::dynamic_pointer_cast<FactSettable<FIRST_STORED_TYPE>>(toRet/*->GetValueShared()*/);
		}
			throw std::logic_error("Value of the fact is not settable.");
	}

	template <typename COST, typename FIRST_STORED_TYPE>
	std::shared_ptr<FactSettable<FIRST_STORED_TYPE, COST>> FactsRepository<COST, FIRST_STORED_TYPE>::GetSettableFact(const std::string& name, const StoredType*)
	{
		auto toRet = storage.at(name);
		if (storage->IsSettable())
		{
			return toRet;
		}
			throw std::logic_error("Value of the fact is not settable.");
	}
}
#endif // universal_reasoner_factsRepository_h__
