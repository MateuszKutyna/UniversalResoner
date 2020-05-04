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

using namespace ureasoner::importer;

std::vector<string> allowedTypes{ "EnumType","BasicType" };

json::const_iterator DigToNode(const json& currentNode, const vector<string>& labels)
{
	auto newNode = currentNode.find(labels[0]);
	for (size_t i = 1; i < labels.size(); i++)
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
	return newNode;
}

std::string ExtractStringValue(const json& currentNode, const string& label)
{
	auto strDigged = DigToNode(currentNode, label)->dump();
	strDigged.erase(std::remove(strDigged.begin(), strDigged.end(), '"'), strDigged.end());
	return strDigged;
}

std::string ExtractOneOfStringValue(const json& currentNode, const std::vector<string>& onesToFind, const string& label)
{
	string strDigged;
	for(const auto& one: onesToFind)
	{
		auto found = DigToNode(currentNode, one);
		if (found!=currentNode.end())
		{
			strDigged = DigToNode(*found,label)->dump();
			strDigged.erase(std::remove(strDigged.begin(), strDigged.end(), '"'), strDigged.end());
			break; //breaks if at least one in collection found
		}
		throw std::invalid_argument("None of labels in the list found");
	}
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

void AddFact(std::vector<ImportedPremise> &premises, const json& value)
{
	auto premise = PrintSingleCondition(value);
	premises.push_back(ImportedPremise({premise.first, premise.second}));
}

void AddFact(std::vector<ImportedConclusion> &conclusions, const json& value)
{
	auto conclusion = PrintSingleCondition(value);
	conclusions.push_back(ImportedConclusion({conclusion.first, conclusion.second}));
}

vector<ImportedFact> ReadFactsIntoContainer(json::const_iterator facts)
{
	vector<ImportedFact> factsContainer;
	for (auto& [key, value] : facts->items())
	{
		auto id = ExtractStringValue(value, "@Id");
		auto type = ExtractOneOfStringValue(value, allowedTypes , "@IdRef");
		factsContainer.push_back({ id,type });
	}
	return std::move(factsContainer);
}

vector<ImportedRule> ReadRulesIntoContainer(json::const_iterator rules)
{
	vector <ImportedRule> rulesStorage;
	for (auto& [key, value] : rules->items())
	{
		auto ifs = value.find("If");
		auto thens = value.find("Then");
		std::vector<ImportedPremise> premises;
		AddFacts(ifs, premises);
		std::vector<ImportedConclusion> conclusions;
		AddFacts(thens, conclusions);
		rulesStorage.push_back(ImportedRule({ premises, conclusions }));
	}
	return std::move(rulesStorage);
}


// Why here?


std::vector<ureasoner::importer::ImportedFact> ureasoner::importer::ReadFactsFromFirstRulesSetRebitJSON(const std::string& filename)
{
	auto j = ReadJson(filename);
	auto facts = DigToNode(j, { "RuleSets" , "RuleSet", "Variables" });
	return ReadFactsIntoContainer(facts);
}

std::vector<ureasoner::importer::ImportedFact> ureasoner::importer::ReadFactsFromRebitJSON(const std::string& filename, const std::string& rulesetName)
{
	auto j = ReadJson(filename);
	auto rulesets = DigToNode(j, "RuleSets");
	for (auto& [key, value] : rulesets->items())
	{
		auto id = ExtractStringValue(value, "@Id");
		if (id==rulesetName)
		{
			auto facts = DigToNode(value, "Variables"); //many rulestes!
			return vector<ImportedFact>(ReadFactsIntoContainer(facts));
			break;	//the rulesSet name is unique
		}
		throw std::invalid_argument("Ruleset with name " + rulesetName + "not found");
	}
	return std::vector<ureasoner::importer::ImportedFact>();
}

vector<ureasoner::importer::ImportedRule> ureasoner::importer::ReadRulesFromFirstRulesSetRebitJSON(const std::string& filename)
 {
 	auto j = ReadJson(filename);
 	auto rules = DigToNode(j, { "RuleSets" , "RuleSet", "Rules" });
 	vector <ImportedRule> rulesStorage;
// 
 	for (auto&[key, value] : rules->items())
 	{
 		auto ifs = value.find("If");
 		auto thens = value.find("Then");
// 
 		std::vector<ImportedPremise> premises;
 		AddFacts(ifs, premises);
// 
 		std::vector<ImportedConclusion> conclusions;
 		AddFacts(thens, conclusions);
// 
 		rulesStorage.push_back(ImportedRule({premises, conclusions}));
 	}
 	return rulesStorage;
 }


vector<ureasoner::importer::ImportedRule> ureasoner::importer::ReadRulesFromRebitJSON(const std::string& filename, const std::string& rulesetName)
{
	auto j = ReadJson(filename);
	auto rulesets = DigToNode(j, "RuleSets");
	for (auto& [key, value] : rulesets->items())
	{
		auto id = ExtractStringValue(value, "@Id");
		if (id == rulesetName)
		{
			auto rules = DigToNode(value, "Rules");
			return vector<ImportedRule>(ReadRulesIntoContainer(rules));
			break;	//the rulesSet name is unique
		}
		throw std::invalid_argument("Ruleset with name " + rulesetName + "not found");
	}
	return std::vector<ureasoner::importer::ImportedRule>();
}
// 
// 
// 
