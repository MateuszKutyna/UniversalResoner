#ifndef knowledgeImporters_knowledgeImporters_h__
#define knowledgeImporters_knowledgeImporters_h__

#include <vector>
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
			vector<ImportedPremise> premises;
			vector<ImportedConclusion> conclusions;
		};

		vector<ImportedFact> ReadFactsFromRebitJSON(const std::string& filename);
		vector<ImportedRule> ReadRulesFromRebitJSON(const std::string& filename);

	}
}

#endif // knowledgeImporters_knowledgeImporters_h__
