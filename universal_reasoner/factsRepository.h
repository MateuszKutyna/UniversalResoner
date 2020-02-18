#ifndef universal_reasoner_factsRepository_h__
#define universal_reasoner_factsRepository_h__

#include "fact.h"

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
		std::shared_ptr< FactWithValue<StoredType>> AddFact(FactWithValue<StoredType> fact, const std::string& name);
	protected:
		std::unordered_map<std::string, std::shared_ptr< FactWithValue<StoredType>>> storage;
		std::shared_ptr< FactWithValue<StoredType>> GetFact(const std::string& name, const StoredType*);
	};

	template <typename FIRST_STORED_TYPE>
	class FactsRepository< FIRST_STORED_TYPE>
	{
	public:
		using StoredType = FIRST_STORED_TYPE;

		template<typename T>
		std::shared_ptr< FactWithValue<StoredType>> GetFactByName(const std::string& name);
		std::shared_ptr< FactWithValue<StoredType>> AddFact(FactWithValue<StoredType> fact, const std::string& name);
	protected:
		std::unordered_map<std::string, std::shared_ptr< FactWithValue<StoredType>>> storage;
		std::shared_ptr< FactWithValue<StoredType>> GetFact(const std::string& name, const StoredType*);
	};

	//????????????????????????????????????? IMPLEMENTATION //////////////////////////////////////////////////////////////////////////////////

	template <typename FIRST_STORED_TYPE>
	std::shared_ptr<FactWithValue<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE>::AddFact(FactWithValue<FIRST_STORED_TYPE> fact, const std::string& name)
	{
		auto toRet = storage.insert(std::pair<std::string, std::shared_ptr< FactWithValue<StoredType>>>(name, std::make_shared<FactWithValue<StoredType>>(fact))).first->second;
		return toRet;
	}

	template <typename FIRST_STORED_TYPE>
	std::shared_ptr<FactWithValue<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE>::GetFact(const std::string& name, const StoredType*)
	{
		return storage.at(name);
	}

	template <typename FIRST_STORED_TYPE>
	template<typename T>
	std::shared_ptr<FactWithValue<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE>::GetFactByName(const std::string& name)
	{
		static_assert(std::is_same<T, StoredType>::value, "FactRepository do not store this type");
		T* trait = nullptr;
		return GetFact(name, trait);
	};

	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	std::shared_ptr<FactWithValue<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE, STORED_TYPES...>::AddFact(FactWithValue<FIRST_STORED_TYPE> fact, const std::string& name)
	{
		auto toRet = storage.insert(std::pair<std::string, std::shared_ptr< FactWithValue<StoredType>>>(name, std::make_shared<FactWithValue<StoredType>>(fact))).first->second;
		return toRet;
	}

	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	std::shared_ptr<FactWithValue<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE, STORED_TYPES...>::GetFact(const std::string& name, const FIRST_STORED_TYPE*)
	{
		return storage.at(name);
	}

	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	template<typename T>
	auto FactsRepository<FIRST_STORED_TYPE, STORED_TYPES...>::GetFactByName(const std::string& name)
	{
		T* trait = nullptr;
		return GetFact(name, trait);
	}

}
#endif // universal_reasoner_factsRepository_h__
