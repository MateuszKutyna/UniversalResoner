#ifndef knowledge_importers_knowledge_importers_h__
#define knowledge_importers_knowledge_importers_h__

#include <vector>
#include "knowledge_importers.h"
#include "../universal_reasoner/rule.h"

namespace reasoner
{
	namespace importer
	{
//		std::vector < shared_ptr<reasoner::Rule>> ReadRulesFromRebitJSON(const std::string& filename);
	}
}

#endif // knowledge_importers_knowledge_importers_h__



// vector < shared_ptr<Rule>> importer::ReadRulesFromRebitJSON(const string& filename)
// {
// 	auto j = ReadJson(filename);
// 	auto rules = DigToNode(j, { "RuleSets" , "RuleSet", "Rules" });
// 	std::vector < shared_ptr<reasoner::Rule>> rulesStorage;
// 
// 	for (auto&[key, value] : rules->items())
// 	{
// 		auto ifs = value.find("If");
// 		auto thens = value.find("Then");
// 
// 		std::vector<shared_ptr<reasoner::Premise>> premises;
// 		AddFacts(ifs, premises);
// 
// 		std::vector<shared_ptr<reasoner::Conclusion>> conclusions;
// 		AddFacts(thens, conclusions);
// 
// 		rulesStorage.push_back(std::make_shared<reasoner::Rule>(reasoner::Rule(premises, conclusions)));
// 	}
// 	return rulesStorage;
// }
