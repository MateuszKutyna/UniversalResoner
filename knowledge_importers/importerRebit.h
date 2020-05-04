#ifndef knowledgeImporters_importerRebit_h__
#define knowledgeImporters_importerRebit_h__

#include <vector>
#include <nlohmann/json.hpp>
#include "knowledge_importers.h"

 void AddFact(std::vector<ureasoner::importer::ImportedPremise> &premises, const nlohmann::json& value);
 void AddFact(std::vector<ureasoner::importer::ImportedConclusion > &conclusions, const nlohmann::json& value);

namespace ureasoner
{
	namespace importer
	{
		template<typename T>
		void AddFacts(nlohmann::json::const_iterator jsonNode, std::vector<T>& container)
		{
			const auto items = jsonNode->size();
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

