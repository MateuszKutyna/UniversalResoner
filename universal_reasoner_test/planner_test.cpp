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
	auto facts = ureasoner::importer::ReadFactsFromRebitJSON("JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(facts.size(), 5);

	auto rules = ureasoner::importer::ReadRulesFromRebitJSON("JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(rules.size(), 18);




	auto repo = make_shared<FactsRepository<COST, std::string>>();
	auto data = make_shared<Metadata<FactsRepository<COST, std::string>>>(repo);

	

	std::map<std::string, shared_ptr<FactWrapperInterface<Metadata<FactsRepository<COST, std::string>>>>> factsMap;
// 
	AddFacts(facts, factsMap, *data);
	AddRules(rules, factsMap, *data);

	auto  ress = repo->GetFactByName<string>("StanZdrowia");
	ress->SetValue("zly");


	Planner< Metadata<FactsRepository<double, std::string>>> plan(data);
	auto availableRules = plan.BuildPlan();
	EXPECT_EQ(availableRules.size(),1); 

	ress = repo->GetFactByName<string>("Wiek");
	ress->SetValue("ponizej 25 lat");

	ress = repo->GetFactByName<string>("Plec");
	ress->SetValue("M");

	availableRules = plan.BuildPlan();
	EXPECT_EQ(availableRules.size(), 6);

	ress = repo->GetFactByName<string>("StanCywilny");
	ress->SetValue("wolny");

	availableRules = plan.BuildPlan();
	EXPECT_EQ(availableRules.size(), 18);
}