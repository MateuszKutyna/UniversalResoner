#ifndef planner_h__
#define planner_h__

#include "metadata.h"

namespace ureasoner
{
	template<typename METADATA>
	class Planer
	{
	public:
		using Metadata = METADATA;
		Planer(shared_ptr<Metadata> metadata) : metadata(metadata) {}
		void BuildPlan();
	protected:
		const shared_ptr<Metadata> metadata;
	};

	template<typename METADATA>
	void ureasoner::Planer<METADATA>::BuildPlan()
	{
		auto facts = metadata->GetKnownFacts();
		auto rules = metadata->GetRules();

		// vector to be sorted

		for each (shared_ptr<Rule<Metadata::CostType>> rule in rules)
		{
			auto neededFacts = rule->GetFactsForFiring();
			auto providedFacts = rule->GetFactsConcluding();
		}

		// Find allowable rules
		// Sort with cost
		// Fire cheapest
		// Register new facts if any
		// Go to find allowable rules.
	}

}
#endif // planner_h__
