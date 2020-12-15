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

		std::multimap < CostType, shared_ptr<Rule<CostType>>> BuildPlan() {

			//all facts that aren't settable = true or fact that are FactConst type
			auto facts = metadata->GetKnownFacts();
			//All rules (18)
			auto rules = metadata->GetRules();

			//Map with keys that aren't unique
			std::multimap < CostType, shared_ptr<Rule<CostType>>> allowedRules;

			for (const auto& rule : rules)
			{
				auto neededFacts = rule->GetFactsForFiring();
				auto providedFacts = rule->GetFactsConcluding();

				bool allNeededFactsAvailable = true;
				auto factIter = neededFacts.cbegin();
				while (factIter != neededFacts.cend() && allNeededFactsAvailable)
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
		};

	protected:

		const shared_ptr<Metadata> metadata;
	};
}
#endif // planner_h__
