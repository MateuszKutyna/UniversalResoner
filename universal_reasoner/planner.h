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

			//all facts that aren't settable = true or facts that are FactConst type
			auto facts = metadata->GetKnownFacts();
			//All rules (18)
			auto rules = metadata->GetRules();

			//Map with keys that aren't unique
			std::multimap < CostType, shared_ptr<Rule<CostType>>> allowedRules;
			//TUTAJ TEZ MOZNA SPROWBOWAC COS ZROWNOLEGLIC
			for (const auto& rule : rules)
			{
				//Bierze fakty ktore sa potrzebne do spelnienia premise nie wszystkie musza byc znane
				auto neededFacts = rule->GetFactsForFiring();

				//Zwraca fakt ktory ma byc ustawiony
				auto providedFacts = rule->GetFactsConcluding(); //Nie u¿yte?? 

				bool allNeededFactsAvailable = true;
				auto factIter = neededFacts.cbegin();
				while (factIter != neededFacts.cend() && allNeededFactsAvailable)
				{
					auto found = std::find(facts->cbegin(), facts->cend(), *factIter);
					if (found == facts->cend()) allNeededFactsAvailable = false;
					++factIter;
				}

				//Dodaje spelniona regule tylko wtedy gdy wszystkie potrzebne fakty do jej wypelnienia sa dostepne
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
