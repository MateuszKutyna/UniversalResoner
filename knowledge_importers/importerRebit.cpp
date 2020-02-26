#include "importerRebit.h"
#include <fstream>
#include <initializer_list>
#include <nlohmann/json.hpp>
#include "../universal_reasoner/rule.h"
#include "../universal_reasoner/fact.h"

using std::vector;
using namespace ureasoner;
using std::string;
using json = nlohmann::json;
using equalPair = std::pair<std::string, std::string>;

json::const_iterator DigToNode(const json& currentNode, const vector<string>& labels)
{
	auto newNode = currentNode.find(labels[0]);
	for (int i = 1; i < labels.size(); i++)
	{
		newNode = newNode->find(labels[i]);
	}
	return newNode;
}

json::const_iterator DigToNode(const json& currentNode, const std::initializer_list<string>& labels)
{
	return DigToNode(currentNode, std::vector<string>(labels));
}

json::const_iterator DigToNode(const json& currentNode, const string& label)
{
	auto newNode = currentNode.find(label);
	auto strOper = newNode->dump();
	return newNode;
}

std::string ExtractStringValue(const json& currentNode, const string& label)
{
	auto strDigged = DigToNode(currentNode, label)->dump();
	strDigged.erase(std::remove(strDigged.begin(), strDigged.end(), '"'), strDigged.end());
	return strDigged;
}

std::string ExtractStringValue(const json& currentNode, const std::initializer_list<string>& labels)
{
	auto strDigged = DigToNode(currentNode, labels)->dump();
	strDigged.erase(std::remove(strDigged.begin(), strDigged.end(), '"'), strDigged.end());
	return strDigged;
}

equalPair PrintSingleCondition(const json& tmp)
{
	string strVarName = ExtractStringValue(tmp, { "LeftHand", "Term", "Variable", "@IdRef" });
	string strVal = ExtractStringValue(tmp, { "RightHand", "Term", "Constant", "String" });
	if (ExtractStringValue(tmp, "@Operator") == "eq")
	{
		return equalPair{ strVarName,strVal };
	}
	else
	{
		throw std::invalid_argument("Only equal comparators available");
	}
}

json ReadJson(const string& filename)
{
	std::ifstream i(filename);
	json j;
	i >> j;
	return j;
}

// void AddFact(std::vector<shared_ptr<ureasoner::Premise>> &premises, const json& value)
// {
// 	auto premise = PrintSingleCondition(value);
// 	premises.push_back(make_shared<ureasoner::StringPremise>(premise.first, premise.second));
// }
// 
// void AddFact(std::vector<shared_ptr<ureasoner::Conclusion>> &conclusions, const json& value)
// {
// 	auto conclusion = PrintSingleCondition(value);
// 	conclusions.push_back(make_shared<ureasoner::ConclusionAddingFact>(std::make_shared<reasoner::FactSourceString>(conclusion.first), conclusion.second));
// }
// 


std::vector<ureasoner::importer::ImportedFact> ureasoner::importer::ReadFacts(const std::string& filename)
{
	auto j = ReadJson(filename);
	auto facts = DigToNode(j, { "RuleSets" , "RuleSet", "Variables" });
	vector<ImportedFact> factsContainer;
	// 
	for (auto& [key, value] : facts->items())
	{
		auto id = ExtractStringValue(value, "@Id");
		auto type = ExtractStringValue(value, { "EnumType", "@IdRef" });
		factsContainer.push_back({ id,type });
	}
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
	 	return factsContainer;

}

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
// 
// 
// 
