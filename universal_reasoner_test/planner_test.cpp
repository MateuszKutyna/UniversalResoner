#include "gtest/gtest.h"
#include "../knowledge_importers/knowledge_importers.h"
#include "../universal_reasoner/factsRepository.h"
#include <string>
#include "../universal_reasoner/Import.h"
// #include "../universal_reasoner/metadata.h"
#include "../universal_reasoner/planner.h"

using std::string;
using std::make_shared;
using namespace ureasoner;
using COST = double;
TEST(BasicPlanner, ImportFromFile)
{	
	//upload fact from JDUDa, (vector<ImportedFact>)
	auto facts = ureasoner::importer::ReadFactsFromFirstRulesSetRebitJSON("JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(facts.size(), 5);

	//upload rules from JDUDa, (vector<ImportedRule>) 
	auto rules = ureasoner::importer::ReadRulesFromFirstRulesSetRebitJSON("JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(rules.size(), 18);

	
	//empty repo unordered_map
	auto repo = make_shared<FactsRepository<COST, std::string>>();

	//empty data, has factRepo(ptr) and rules(vector)
	//it is container for facts repo and rules
	auto data = make_shared<Metadata<FactsRepository<COST, std::string>>>(repo);

	//Inserts facts into repo and into a map
	auto map = AddFacts(facts, *repo);

	//Creating rules
	AddRules(rules, *data, map);

	//Sets fact as "StanZdrowia jest zly" with a COST = 5.0
	auto  ress = repo->GetFactByName<string>("StanZdrowia");
	ress->SetValue("zly");
	ress->SetCost(5.0);
	

 	Planner< Metadata<FactsRepository<COST, std::string>>> plan(data);
	auto availableRules = plan.BuildPlan();
	EXPECT_EQ(availableRules.size(),1); 

	ress = repo->GetFactByName<string>("Wiek");
	ress->SetValue("ponizej 25 lat");
	ress->SetCost(10.0);

	ress = repo->GetFactByName<string>("Plec");
	ress->SetValue("M");
	ress->SetCost(15.0);

	availableRules = plan.BuildPlan();
	EXPECT_EQ(availableRules.size(), 6);


	ress = repo->GetFactByName<string>("StanCywilny");
	ress->SetValue("wolny");
	ress->SetCost(50.0);

	availableRules = plan.BuildPlan();
	EXPECT_EQ(availableRules.size(), 18);

	//18 rules 
	EXPECT_EQ(availableRules.count(5), 1);
	EXPECT_EQ(availableRules.count(30), 5);
	EXPECT_EQ(availableRules.count(65), 2);
	EXPECT_EQ(availableRules.count(80), 10);

	availableRules.begin()->second->CheckAndFire();

	ress = repo->GetFactByName<string>("OcenaCechOsobowych");
	auto value = ress->GetValue();
	EXPECT_EQ("bardzo niska", value);
}