#ifndef knowledgeImporters_knowledgeImporters_h__
#define knowledgeImporters_knowledgeImporters_h__

#include <vector>
#include "../universal_reasoner/rule.h"
#include <string>

using std::string;
using std::vector;

namespace ureasoner
{
	namespace importer
	{
		struct ImportedFact
		{
			string name;
			string type;
		};

		struct ImportedPremise
		{
			string factName;
			string expectedValue;
		};

		struct ImportedConclusion
		{
			string factName;
			string valueToSet;
		};

		struct ImportedRule
		{
// 			ImportedRule(vector<ImportedPremise>&& p) noexcept :
// 				s(std::move(o.s)),       // explicit move of a member of class type
// 				k(std::exchange(o.k, 0)) // explicit move of a member of non-class type
// 			{ }
			vector<ImportedPremise> premises;
			vector<ImportedConclusion> conclusions;
		};

		vector<ImportedFact> ReadFacts(const std::string& filename);

		vector<ImportedRule> ReadRulesFromRebitJSON(const std::string& filename);
	}
}

#endif // knowledgeImporters_knowledgeImporters_h__
