#include "gtest/gtest.h"
#include "../knowledge_importers/knowledge_importers.h"
#include "../universal_reasoner/factsRepository.h"
#include <string>
#include "../universal_reasoner/Import.h"
#include "../universal_reasoner/metadata.h"
#include "../universal_reasoner/planner.h"

using namespace ureasoner;
using COST = double;
TEST(BasicPlanner, ImportFromFile)
{
	auto facts = ureasoner::importer::ReadFactsFromFirstRulesSetRebitJSON("JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(facts.size(), 5);

	auto rules = ureasoner::importer::ReadRulesFromFirstRulesSetRebitJSON("JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(rules.size(), 18);

	auto repo = make_shared<FactsRepository<COST, std::string>>();
	auto data = make_shared<Metadata<FactsRepository<COST, std::string>>>(repo);

	std::map<std::string, shared_ptr<FactWrapperInterface<Metadata<FactsRepository<COST, std::string>>>>> factsMap;
// 
	auto map = AddFacts(facts, *repo);
	AddRules(rules, *data, map);

// 
	auto  ress = repo->GetFactByName<string>("StanZdrowia");
	ress->SetValue("zly");
	ress->GetValueShared()->SetCost(5.0);
// 
 	Planner< Metadata<FactsRepository<double, std::string>>> plan(data);
	auto availableRules = plan.BuildPlan();
	EXPECT_EQ(availableRules.size(),1); 

	ress = repo->GetFactByName<string>("Wiek");
	ress->SetValue("ponizej 25 lat");
	ress->GetValueShared()->SetCost(10.0);

	ress = repo->GetFactByName<string>("Plec");
	ress->SetValue("M");
	ress->GetValueShared()->SetCost(15.0);

	availableRules = plan.BuildPlan();
	EXPECT_EQ(availableRules.size(), 6);


	ress = repo->GetFactByName<string>("StanCywilny");
	ress->SetValue("wolny");
	ress->GetValueShared()->SetCost(50.0);

	availableRules = plan.BuildPlan();
	EXPECT_EQ(availableRules.size(), 18);

	EXPECT_EQ(availableRules.count(5), 1);
	EXPECT_EQ(availableRules.count(30), 5);
	EXPECT_EQ(availableRules.count(65), 2);
	EXPECT_EQ(availableRules.count(80), 10);

	availableRules.begin()->second->CheckAndFire();

	ress = repo->GetFactByName<string>("OcenaCechOsobowych");
	auto value = ress->GetValue();
	EXPECT_EQ("bardzo niska", value);
}