#ifndef universal_reasoner_factsRepository_h__
#define universal_reasoner_factsRepository_h__

#include "fact.h"
#include <unordered_map>
using std::vector;

namespace ureasoner
{

	template <typename T, typename... List>
	struct IsContained;

	template <typename T, typename Head, typename... Tail>
	struct IsContained<T, Head, Tail...>
	{
		enum { value = std::is_same<T, Head>::value || (IsContained<T, Tail...>::value && /*... && */true) };
	};

	template <typename T>
	struct IsContained<T>
	{
		enum { value = false };
	};


	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	class FactsRepository : public FactsRepository<STORED_TYPES...>
	{
		static_assert(!IsContained<FIRST_STORED_TYPE, STORED_TYPES...>::value, "No duplicate types allowed");

	public:
		using FactsRepository<STORED_TYPES...>::AddFact;
		using FactsRepository<STORED_TYPES...>::GetFact;
		using StoredType = FIRST_STORED_TYPE;

		template<typename T>
		auto GetFactByName(const std::string& name);	// auto here allows to pass a result from inherited GetFact function!
		template<typename T>
		auto GetSettableFactByName(const std::string& name);	// auto here allows to pass a result from inherited GetFact function!
		std::shared_ptr< FactRepresentation<StoredType>> AddFact(FactRepresentation<StoredType> fact, const std::string& name);
		std::shared_ptr< FactRepresentation<StoredType>> AddFact(const StoredType& fact, const std::string& name);
		shared_ptr<vector<shared_ptr<CheckableFact>>> GetAllKnownFacts();


	protected:
		std::unordered_map<std::string, std::shared_ptr< FactRepresentation<StoredType>>> storage;
		std::shared_ptr< FactRepresentation<StoredType>> GetFact(const std::string& name, const StoredType*);
		std::shared_ptr< FactSettable<StoredType>> GetSettableFact(const std::string& name, const StoredType*);
	};

	template <typename FIRST_STORED_TYPE>
	class FactsRepository< FIRST_STORED_TYPE>
	{
	public:
		using StoredType = FIRST_STORED_TYPE;

		template<typename T>
		std::shared_ptr< FactRepresentation<StoredType>> GetFactByName(const std::string& name);
		std::shared_ptr< FactRepresentation<StoredType>> GetSettableFactByName(const std::string& name);
		std::shared_ptr< FactRepresentation<StoredType>> AddFact(FactRepresentation<StoredType> fact, const std::string& name);
		std::shared_ptr< FactRepresentation<StoredType>> AddFact(const StoredType& fact, const std::string& name);
		shared_ptr<vector<shared_ptr<CheckableFact>>> GetAllKnownFacts();

	protected:
		std::unordered_map<std::string, std::shared_ptr< FactRepresentation<StoredType>>> storage;
		std::shared_ptr< FactRepresentation<StoredType>> GetFact(const std::string& name, const StoredType*);
		std::shared_ptr< FactSettable<StoredType>> GetSettableFact(const std::string& name, const StoredType*);
	};

	//????????????????????????????????????? IMPLEMENTATION //////////////////////////////////////////////////////////////////////////////////

	template <typename FIRST_STORED_TYPE>
	shared_ptr<vector<shared_ptr<CheckableFact>>> FactsRepository<FIRST_STORED_TYPE>::GetAllKnownFacts()
	{
		auto toRet = make_shared<vector<shared_ptr<CheckableFact>>>();
		for each (auto fact in storage)
		{
			if (fact.second->GetValueShared()->IsKnown())
			{
				toRet->push_back(fact.second->GetValueShared());
			}
		}
		return toRet;
	}


	template <typename FIRST_STORED_TYPE>
	std::shared_ptr<FactRepresentation<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE>::AddFact(const FIRST_STORED_TYPE& fact, const std::string& name)
	{
		return AddFact(FactRepresentation<FIRST_STORED_TYPE>(fact), name);
	}


	template <typename FIRST_STORED_TYPE>
	std::shared_ptr<FactRepresentation<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE>::AddFact(FactRepresentation<FIRST_STORED_TYPE> fact, const std::string& name)
	{
		auto toRet = storage.insert(std::pair<std::string, std::shared_ptr< FactRepresentation<StoredType>>>(name, std::make_shared<FactRepresentation<StoredType>>(fact))).first->second;
		return toRet;
	}

	template <typename FIRST_STORED_TYPE>
	std::shared_ptr<FactRepresentation<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE>::GetFact(const std::string& name, const StoredType*)
	{
		return storage.at(name);
	}

	template <typename FIRST_STORED_TYPE>
	std::shared_ptr<FactSettable<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE>::GetSettableFact(const std::string& name, const StoredType*)
	{
		auto toRet = storage.at(name);
		if (storage->IsSettable())
		{
			return toRet;
		}
		else
		{
			throw std::logic_error("Value of the fact is not settable.");
		}
	}

	template <typename FIRST_STORED_TYPE>
	template<typename T>
	std::shared_ptr<FactRepresentation<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE>::GetFactByName(const std::string& name)
	{
		static_assert(std::is_same<T, StoredType>::value, "FactRepository do not store this type");
		T* trait = nullptr;
		return GetFact(name, trait);
	};

	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	shared_ptr<vector<shared_ptr<CheckableFact>>> FactsRepository<FIRST_STORED_TYPE, STORED_TYPES...>::GetAllKnownFacts()
	{
		auto toRet = FactsRepository<STORED_TYPES...>::GetAllKnownFacts();
		for each (auto fact in storage)
		{
			if (fact.second->GetValueShared()->IsKnown())
			{
				toRet->push_back(fact.second->GetValueShared());
			}
		}
		return toRet;
	}

	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	std::shared_ptr<FactRepresentation<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE, STORED_TYPES...>::AddFact(const FIRST_STORED_TYPE& fact, const std::string& name)
	{
		return AddFact(FactRepresentation<FIRST_STORED_TYPE>(fact), name);
	}

	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	std::shared_ptr<FactRepresentation<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE, STORED_TYPES...>::AddFact(FactRepresentation<FIRST_STORED_TYPE> fact, const std::string& name)
	{
		auto toRet = storage.insert(std::pair<std::string, std::shared_ptr< FactRepresentation<StoredType>>>(name, std::make_shared<FactRepresentation<StoredType>>(fact))).first->second;
		return toRet;
	}

	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	std::shared_ptr<FactRepresentation<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE, STORED_TYPES...>::GetFact(const std::string& name, const FIRST_STORED_TYPE*)
	{
		return storage.at(name);
	}

	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	std::shared_ptr<FactSettable<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE, STORED_TYPES...>::GetSettableFact(const std::string& name, const FIRST_STORED_TYPE*)
	{
		auto toRet = storage.at(name);
		if (toRet->IsSettable())
		{
			return dynamic_pointer_cast<FactSettable<FIRST_STORED_TYPE>>(toRet->GetValueShared());
		} 
		else
		{
			throw std::logic_error("Value of the fact is not settable.");
		}
	}

	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	template<typename T>
	auto FactsRepository<FIRST_STORED_TYPE, STORED_TYPES...>::GetFactByName(const std::string& name)
	{
		T* trait = nullptr;
		return GetFact(name, trait);
	}

	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	template<typename T>
	auto FactsRepository<FIRST_STORED_TYPE, STORED_TYPES...>::GetSettableFactByName(const std::string& name)
	{
		T* trait = nullptr;
		return GetSettableFact(name, trait);
	}

}
#endif // universal_reasoner_factsRepository_h__
