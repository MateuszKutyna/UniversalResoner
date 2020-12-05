#ifndef planner_h__
#define planner_h__

#include "metadata.h"
#include <map>

namespace ureasoner
{
	template<typename METADATA>
	class Planner
	{
	public:
	
		using Metadata = METADATA;
		using CostType = typename Metadata::CostType;

		Planner(shared_ptr<Metadata> metadata) : metadata(metadata) {}

		std::multimap < typename METADATA::CostType, shared_ptr<Rule<typename METADATA::CostType>>> BuildPlan();
	protected:

		const shared_ptr<Metadata> metadata;
	};

	template<typename METADATA>
	std::multimap < typename METADATA::CostType, shared_ptr<Rule<typename METADATA::CostType>>> ureasoner::Planner<METADATA>::BuildPlan()
	{
		auto facts = metadata->GetKnownFacts();
		auto rules = metadata->GetRules();

		//Map with keys that aren't unique
		std::multimap < CostType, shared_ptr<Rule<CostType>>> allowedRules;

		for (const auto& rule: rules)
		{
			auto neededFacts = rule->GetFactsForFiring();
			auto providedFacts = rule->GetFactsConcluding();

			bool allNeededFactsAvailable = true;
			auto factIter = neededFacts.cbegin();
			while (factIter!= neededFacts.cend() && allNeededFactsAvailable)
			{
				auto found = std::find(facts->cbegin(), facts->cend(), *factIter);
				if (found == facts->cend()) allNeededFactsAvailable = false;
				++factIter;
			}
			if (allNeededFactsAvailable)
			{
				allowedRules.insert(std::pair{ rule->GetEstimatedCost(), rule });
			}
			
		}

		return allowedRules;
		// Find allowable rules
		// Sort with cost
		// Fire cheapest
		// Register new facts if any
		// Go to find allowable rules.
	}

}
#endif // planner_h__
