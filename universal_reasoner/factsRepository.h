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

	template <typename FIRST_STORED_TYPE>
	class FactsRepositoryLevel
	{
	public:
		using StoredType = FIRST_STORED_TYPE;
		// 		template<typename T>
		// 		//std::shared_ptr< FactWithValue<StoredType>> GetFactByName(const std::string& name);
		// 		auto GetFactByName(const std::string& name)
		// 		{
		// 			return std::shared_ptr< FactWithValue<StoredType>>();
		// 		}
		// 		;
		// 		
		// 		template<>
		// 		//std::shared_ptr< FactWithValue<StoredType>> GetFactByName< StoredType>(const std::string& name)
		// 		auto GetFactByName< StoredType>(const std::string& name)
		// 		{
		// 			return storage[name];
		// 		}
		std::shared_ptr< FactWithValue<StoredType>> AddFact(FactWithValue<StoredType> fact, const std::string& name);
	protected:
		std::unordered_map<std::string, std::shared_ptr< FactWithValue<StoredType>>> storage;
	};

	template <typename FIRST_STORED_TYPE>
	std::shared_ptr<FactWithValue<FIRST_STORED_TYPE>> FactsRepositoryLevel<FIRST_STORED_TYPE>::AddFact(FactWithValue<FIRST_STORED_TYPE> fact, const std::string& name)
	{
		storage.insert(std::pair<std::string, std::shared_ptr< FactWithValue<FIRST_STORED_TYPE>>>
			(name, std::make_shared<FactWithValue<FIRST_STORED_TYPE>>(fact)));
		return
			(storage[name]);


	}


	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	class FactsRepository : public FactsRepository<STORED_TYPES...>/*, public FactsRepositoryLevel< FIRST_STORED_TYPE>*/
	{
		static_assert(!IsContained<FIRST_STORED_TYPE, STORED_TYPES...>::value, "No duplicate types allowed");

	public:
		using FactsRepository<STORED_TYPES...>::AddFact;
		using FactsRepository<STORED_TYPES...>::GetFact;
		//		using FactsRepositoryLevel< FIRST_STORED_TYPE>::AddFact;
		//		using FactsRepositoryLevel< FIRST_STORED_TYPE>::GetFactByName;
		using StoredType = FIRST_STORED_TYPE;

		std::shared_ptr< FactWithValue<StoredType>> GetFact(const std::string& name, const StoredType*)
		{
			return storage[name];
		}

		template<typename T>
		//std::shared_ptr< FactWithValue<StoredType>> GetFactByName(const std::string& name);
		auto GetFactByName(const std::string& name)
		{
			T* trait=nullptr;
			return /*FactsRepository<STORED_TYPES...>::*/GetFact(name, trait);
		}

// 		template<>
// 		//std::shared_ptr< FactWithValue<StoredType>> GetFactByName< StoredType>(const std::string& name)
// 		std::shared_ptr< FactWithValue<StoredType>> GetFactByName<StoredType>(const std::string& name)
// 		{
// 			return storage[name];
// 		}
		std::shared_ptr< FactWithValue<StoredType>> AddFact(FactWithValue<StoredType> fact, const std::string& name);
	protected:
		std::unordered_map<std::string, std::shared_ptr< FactWithValue<StoredType>>> storage;
	};

	template <typename FIRST_STORED_TYPE>
	class FactsRepository< FIRST_STORED_TYPE>/* : public FactsRepositoryLevel< FIRST_STORED_TYPE>*/
	{
	public:
		using StoredType = FIRST_STORED_TYPE;

		std::shared_ptr< FactWithValue<StoredType>> GetFact(const std::string& name, const StoredType*)
		{
			return storage[name];
		}


		template<typename T>
		std::shared_ptr< FactWithValue<StoredType>> GetFactByName(const std::string& name)
		//auto GetFactByName(const std::string& name)
		{
			return std::shared_ptr< FactWithValue<StoredType>>();
		}
		;

		template<>
		std::shared_ptr< FactWithValue<StoredType>> GetFactByName< StoredType>(const std::string& name)
		//auto GetFactByName< StoredType>(const std::string& name)
		{
			return storage[name];
		}
		std::shared_ptr< FactWithValue<StoredType>> AddFact(FactWithValue<StoredType> fact, const std::string& name);
	protected:
		std::unordered_map<std::string, std::shared_ptr< FactWithValue<StoredType>>> storage;
	};



	template <typename FIRST_STORED_TYPE>
	std::shared_ptr<FactWithValue<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE>::AddFact(FactWithValue<FIRST_STORED_TYPE> fact, const std::string& name)
	{
		storage.insert(std::pair<std::string, std::shared_ptr< FactWithValue<FIRST_STORED_TYPE>>>
			(name, std::make_shared<FactWithValue<FIRST_STORED_TYPE>>(fact)));
		return
			(storage[name]);


	}

	template <typename FIRST_STORED_TYPE, typename... STORED_TYPES>
	std::shared_ptr<FactWithValue<FIRST_STORED_TYPE>> FactsRepository<FIRST_STORED_TYPE, STORED_TYPES...>::AddFact(FactWithValue<FIRST_STORED_TYPE> fact, const std::string& name)
	{
		storage.insert(std::pair<std::string, std::shared_ptr< FactWithValue<FIRST_STORED_TYPE>>>
			(name, std::make_shared<FactWithValue<FIRST_STORED_TYPE>>(fact)));
		return
			(storage[name]);
	}
}
#endif // universal_reasoner_factsRepository_h__
