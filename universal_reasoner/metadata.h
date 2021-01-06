#ifndef universal_reasoner_metadata_h__
#define universal_reasoner_metadata_h__

#include "fact.h"
#include "executableWithCost.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "rule.h"
#include <concurrent_vector.h>

using std::shared_ptr;
using std::vector;

namespace ureasoner
{
	template<typename FACTS_REPOSITORY, typename COST_TYPE = double, typename EXECUTABLE = ExecutableWithCost<COST_TYPE>>
	class Metadata{
	public:
		using ExecutableWithCost = EXECUTABLE;
		using FactsRepository = FACTS_REPOSITORY;
		using CostType = COST_TYPE;

		void AddRule(shared_ptr<Rule<COST_TYPE>> rule)
		{
			rules.push_back(rule);
		}

		template <typename STORED_TYPE>
		std::shared_ptr< Fact<STORED_TYPE>> AddFact(std::shared_ptr<Fact<STORED_TYPE>> fact, const std::string& name) { return factsRepository->AddFact(fact, name); }
		Metadata(shared_ptr<FactsRepository> factsRepository) : factsRepository(factsRepository) {};

		shared_ptr<Concurrency::concurrent_vector<shared_ptr<CheckableFact<CostType>>>> GetKnownFacts();
		Concurrency::concurrent_vector<shared_ptr<Rule<COST_TYPE>>> GetRules() { return rules; };

		shared_ptr<FactsRepository> GetFactsRepository() { return factsRepository; }

	protected:
		shared_ptr< FactsRepository> factsRepository;
		Concurrency::concurrent_vector<shared_ptr<Rule<COST_TYPE>>> rules;
	};

	template<typename FACTS_REPOSITORY, typename COST_TYPE /*= double*/, typename EXECUTABLE /*= ExecutableWithCost<COST_TYPE>*/>
	shared_ptr<Concurrency::concurrent_vector<shared_ptr<CheckableFact<COST_TYPE>>>> ureasoner::Metadata<FACTS_REPOSITORY, COST_TYPE, EXECUTABLE>::GetKnownFacts()
	{
		return factsRepository->GetAllKnownFacts();
	}

}

#endif // universal_reasoner_metadata_h__
