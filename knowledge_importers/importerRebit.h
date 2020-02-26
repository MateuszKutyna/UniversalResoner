#ifndef knowledgeImporters_importerRebit_h__
#define knowledgeImporters_importerRebit_h__

#include <nlohmann/json.hpp>
#include "knowledge_importers.h"
#include "../universal_reasoner/rule.h"
#include <vector>

using std::vector;
using std::shared_ptr;


 void AddFact(vector<ureasoner::importer::ImportedPremise> &premises, const nlohmann::json& value);
 void AddFact(vector<ureasoner::importer::ImportedConclusion > &conclusions, const nlohmann::json& value);

namespace ureasoner
{
	namespace importer
	{


		template<typename T>
		void AddFacts(nlohmann::json::const_iterator jsonNode, vector<T>& container)
		{
			auto items = jsonNode->size();
			if (items == 1) //atom
			{
				auto tmp = jsonNode->find("Atom");
				AddFact(container, *tmp);
			}
			else //list
			{
				for (auto&[key, value] : jsonNode->items())
				{
					AddFact(container, value);
				}
			}
		}

	}
}
#endif // knowledgeImporters_importerRebit_h__

