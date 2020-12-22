
#include <iostream>
#include <factsRepository.h>
#include <Import.h>
#include <metadata.h>
#include <memory>
#include <planner.h>
#include <string>
#include<type_traits>
#include<concepts>
#include<string>
#include<fstream>
#include<chrono>

using namespace ureasoner;
using COST = double;

//TIMER 
struct Timer {
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<float> duration;

	Timer() {
		start = std::chrono::high_resolution_clock::now();
	}

	~Timer() {
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;

		float ms = duration.count() * 1000.0f;
		std::cout << "Timer took " << ms << "ms " << std::endl;
	}
};


int main()
{
	Timer timer;

	auto facts = ureasoner::importer::ReadFactsFromFirstRulesSetRebitJSON("../JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	
	auto rules = ureasoner::importer::ReadRulesFromFirstRulesSetRebitJSON("../JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	
	auto repo = std::make_shared<FactsRepository<COST, std::string>>();

	auto data = std::make_shared<Metadata<FactsRepository<COST, std::string>>>(repo);

	auto map = AddFacts(facts, *repo);

	AddRules(rules, *data, map);

	Planner< Metadata<FactsRepository<COST, std::string>>> plan(data);

	auto  ress = repo->GetFactByName<std::string>("StanZdrowia");
	ress->SetValue("dobry");
	ress->SetCost(10.0);

	ress = repo->GetFactByName<std::string>("Wiek");
	ress->SetValue("powyzej 40 lat");
	ress->SetCost(20.0);

	ress = repo->GetFactByName<std::string>("Plec");
	ress->SetValue("K");
	ress->SetCost(30.0);

	ress = repo->GetFactByName<std::string>("StanCywilny");
	ress->SetValue("zajety");
	ress->SetCost(40.0);

	auto availableRules = plan.BuildPlan();


	auto rule = availableRules.cbegin();
	while (rule != availableRules.cend()) {
		rule->second->CheckAndFire();
		rule++;
	}

	ress = repo->GetFactByName<std::string>("OcenaCechOsobowych");
	auto value = ress->GetValue();
	std::cout << value<<std::endl;
}

