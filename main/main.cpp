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

void test_1() {
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

	Concurrency::parallel_for_each(std::begin(availableRules), std::end(availableRules), [&](auto rule) {
		rule.second->CheckAndFire();
		});

	/*for (auto rule : availableRules) {
		rule.second->CheckAndFire();
	}*/

	

	ress = repo->GetFactByName<std::string>("OcenaCechOsobowych");
	auto value = ress->GetValue();
	std::cout << value << std::endl;
}

void test_2() {
	Timer timer;
	auto facts = ureasoner::importer::ReadFactsFromFirstRulesSetRebitJSON("../Rules.json"); //copy JDuda to universal reasoner\x64\Debug

	auto rules = ureasoner::importer::ReadRulesFromFirstRulesSetRebitJSON("../Rules.json"); //copy JDuda to universal reasoner\x64\Debug

	auto repo = std::make_shared<FactsRepository<COST, std::string>>();

	auto data = std::make_shared<Metadata<FactsRepository<COST, std::string>>>(repo);

	auto map = AddFacts(facts, *repo);

	AddRules(rules, *data, map);

	Planner< Metadata<FactsRepository<COST, std::string>>> plan(data);
	auto  ress = repo->GetFactByName<std::string>("Zmienna_a");
	ress->SetValue("a");
	ress = repo->GetFactByName<std::string>("Zmienna_b");
	ress->SetValue("b");

	while (!repo->GetFactByName<std::string>("Zmienna_g")->IsKnown()) {
		auto avilable_rules = plan.BuildPlan();

		//Sekwencyjne
		/*for (auto rule : avilable_rules) {
		rule.second->CheckAndFire();
		}*/
		//Rownolegle
		Concurrency::parallel_for_each(std::begin(avilable_rules), std::end(avilable_rules), [&](auto rule) {
			rule.second->CheckAndFire();
			});
	}
	std::cout<< repo->GetFactByName<std::string>("Zmienna_a")->GetValue()  << "\n";
	std::cout << repo->GetFactByName<std::string>("Zmienna_b")->GetValue() << "\n";
	std::cout << repo->GetFactByName<std::string>("Zmienna_c")->GetValue() << "\n";
	std::cout << repo->GetFactByName<std::string>("Zmienna_d")->GetValue() << "\n";
	std::cout << repo->GetFactByName<std::string>("Zmienna_e")->GetValue() << "\n";
	std::cout << repo->GetFactByName<std::string>("Zmienna_f")->GetValue() << "\n";
	std::cout << repo->GetFactByName<std::string>("Zmienna_g")->GetValue() << "\n";
}

int main()
{
	//JDUDA
	//test_1();
	//RULES
	test_2();
}