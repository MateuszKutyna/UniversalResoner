#ifndef universal_reasoner_metadata_h__
#define universal_reasoner_metadata_h__

#include "fact.h"
#include "executableWithCost.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "rule.h"
#include <vector>

using std::shared_ptr;
using std::vector;

namespace ureasoner
{
	template<typename FACTS_REPOSITORY, typename COST_TYPE = double, typename EXECUTABLE = ExecutableWithCost<COST_TYPE>>
	class Metadata
	{
	public:
		using ExecutableWithCost = EXECUTABLE;
		using FactsRepository = FACTS_REPOSITORY;

		void AddRule(shared_ptr<Rule<COST_TYPE>> rule)
		{
			rules.push_back(rule);
		}

		Metadata(shared_ptr<FactsRepository> factsRepository) : factsRepository(factsRepository) {};
// 		std::unordered_map<Fact, bool> GetAllFacts();
// 		std::unordered_set<Fact> GetKnownFacts();
// 		std::vector<ExecutableWithCost> GetExecutablesProvidingFact(const Fact& fact);
// 		std::vector<ExecutableWithCost> GetExecutablesUsingFact(const Fact& fact);
// 		std::vector<ExecutableWithCost> GetExecutablesUsingOnlyFacts(const std::vector<Fact> facts);
	protected:
		shared_ptr< FACTS_REPOSITORY> factsRepository;
		vector<shared_ptr<Rule<COST_TYPE>>> rules;
	};
}

#endif // universal_reasoner_metadata_h__
