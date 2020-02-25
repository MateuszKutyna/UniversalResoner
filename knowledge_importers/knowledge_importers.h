#ifndef knowledgeImporters_knowledgeImporters_h__
#define knowledgeImporters_knowledgeImporters_h__

#include <vector>
#include "../universal_reasoner/rule.h"
#include <string>

using std::string;

namespace ureasoner
{
	namespace importer
	{
		struct ImportedFact
		{
			string name;
			string type;
		};
		std::vector<ImportedFact> ReadFacts(const std::string& filename);

		//std::vector < shared_ptr<reasoner::Rule>> ReadRulesFromRebitJSON(const std::string& filename);
	}
}

#endif // knowledgeImporters_knowledgeImporters_h__
