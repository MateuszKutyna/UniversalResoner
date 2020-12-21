
#include <iostream>
#include <factsRepository.h>
#include <import.h>
#include <metadata.h>
#include <memory>
#include <planner.h>
#include <string>
#include<type_traits>
#include<concepts>
#include<string>
#include<fstream>

using namespace ureasoner;
using COST = double;

int main()
{

	auto facts = ureasoner::importer::ReadFactsFromRebitJSON("../JDuda.json", "CechyOsobowe_rules");

	for (const auto& fact : facts) {
		std::cout << "Name: " << fact.name << " Type: " << fact.type << std::endl;
	}

	auto rules = ureasoner::importer::ReadRulesFromRebitJSON("../JDuda.json", "CechyOsobowe_rules"); //copy JDuda to universal reasoner\x64\Debug
	
	for (const auto& rule : rules) {
		std::cout << "If ";
		for (const auto& premise : rule.premises) {
			std::cout << premise.expectedValue << " ";

		}
		std::cout << " Then ";
		for (const auto& conclusion : rule.conclusions) {
			std::cout << conclusion.valueToSet;
		}
		std::cout << "\n";
	}
	
	
}

