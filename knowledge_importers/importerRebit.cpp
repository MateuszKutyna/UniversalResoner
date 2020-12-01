#include "importerRebit.h"
#include <fstream>
#include <iostream>
#include <initializer_list>
#include <algorithm>
#include <nlohmann/json.hpp>

using std::vector;
using std::string;
using json = nlohmann::json;
using equalPair = std::pair<std::string, std::string>;

using namespace ureasoner;
using namespace ureasoner::importer;

vector<string> allowedTypes{"EnumType","BasicType"}; //vector of strings Holding Type of types 

// DigToNode extrects categories From JSON file
json::const_iterator DigToNode(const json& currentNode, const vector<string>& labels)
{
	auto newNode = currentNode.find(labels.front());
	//Goes into the deep of JSON file
	std::for_each(labels.cbegin() + 1, labels.cend(), [&newNode](auto& label) {newNode = newNode->find(label); });
	return newNode;
}

json::const_iterator DigToNode(const json& currentNode, const std::initializer_list<string>& labels)
{
	return DigToNode(currentNode, vector<string>(labels));
}

json::const_iterator DigToNode(const json& currentNode, const string& label)
{
	return currentNode.find(label);
}

string ExtractStringValue(const json& currentNode, const string& label)
{
	auto strDigged = DigToNode(currentNode, label)->dump();
	strDigged.erase(std::remove(strDigged.begin(), strDigged.end(), '"'), strDigged.end()); //Deletes " from string
	return strDigged;
}

string ExtractOneOfStringValue(const json& currentNode, const vector<string>& onesToFind, const string& label)
{
	string strDigged("");
	for (const auto& one : onesToFind)
	{
		auto found = DigToNode(currentNode, one);
		if (found != currentNode.end())
		{
			strDigged = ExtractStringValue(*found, label);
			break; //breaks if at least one in collection found
		}
		throw std::invalid_argument("None of labels in the list found");
	}
	return strDigged;
}

string ExtractStringValue(const json& currentNode, const std::initializer_list<string>& labels)
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

void AddFact(vector<ImportedPremise>& premises, const json& value)
{
	auto premise = PrintSingleCondition(value);
	premises.push_back(ImportedPremise({ premise.first, premise.second }));
}

void AddFact(vector<ImportedConclusion>& conclusions, const json& value)
{
	auto conclusion = PrintSingleCondition(value);
	conclusions.push_back(ImportedConclusion({ conclusion.first, conclusion.second }));
}

vector<ImportedFact> ReadFactsIntoContainer(json::const_iterator facts)
{
	//Takes facts from Variables
	vector<ImportedFact> factsContainer;
	for (auto& [key, value] : facts->items())
	{
		//@ID e.g. OcenaCechOsobowych
		auto id = ExtractStringValue(value, "@Id");
		//@IdRef e.g. ocena
		auto type = ExtractOneOfStringValue(value, allowedTypes, "@IdRef");
		factsContainer.push_back({ id,type });
	}
	//returns vector of ImportedFacts
	return std::move(factsContainer);
}

vector<ImportedRule> ReadRulesIntoContainer(json::const_iterator rules)
{
	vector <ImportedRule> rulesStorage;
	for (auto& [key, value] : rules->items())
	{
		auto ifs = value.find("If");
		auto thens = value.find("Then");

		vector<ImportedPremise> premises;
		AddFacts(ifs, premises);

		vector<ImportedConclusion> conclusions;
		AddFacts(thens, conclusions);
		
		rulesStorage.push_back(ImportedRule({ premises, conclusions }));
	}
	return std::move(rulesStorage);
}

vector<ureasoner::importer::ImportedFact> ureasoner::importer::ReadFactsFromFirstRulesSetRebitJSON(const std::string& filename)
{
	auto j = ReadJson(filename);
	auto facts = DigToNode(j, { "RuleSets" , "RuleSet", "Variables" });
	return ReadFactsIntoContainer(facts);
}

vector<ureasoner::importer::ImportedFact> ureasoner::importer::ReadFactsFromRebitJSON(const std::string& filename, const std::string& rulesetName)
{
	auto j = ReadJson(filename);
	auto rulesets = DigToNode(j, "RuleSets");
	
	//Reads facts from RuleSets Variables
	for (auto& [key, value] : rulesets->items())
	{
		auto id = ExtractStringValue(value, "@Id");
		if (id == rulesetName)
		{
			auto facts = DigToNode(value, "Variables"); //many rulesets!
			return vector<ImportedFact>(ReadFactsIntoContainer(facts));
			break;	//the rulesSet name is unique
		}
		throw std::invalid_argument("Ruleset with name " + rulesetName + "not found");
	}

	return vector<ureasoner::importer::ImportedFact>();
}

vector<ureasoner::importer::ImportedRule> ureasoner::importer::ReadRulesFromFirstRulesSetRebitJSON(const std::string& filename)
{
	auto j = ReadJson(filename);
	auto rules = DigToNode(j, { "RuleSets" , "RuleSet", "Rules" });
	vector <ImportedRule> rulesStorage;
	for (auto& [key, value] : rules->items())
	{
		auto ifs = value.find("If");
		auto thens = value.find("Then");

		vector<ImportedPremise> premises;
		AddFacts(ifs, premises);

		vector<ImportedConclusion> conclusions;
		AddFacts(thens, conclusions);

		rulesStorage.push_back(ImportedRule({ premises, conclusions }));
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
	return vector<ureasoner::importer::ImportedRule>();
}
