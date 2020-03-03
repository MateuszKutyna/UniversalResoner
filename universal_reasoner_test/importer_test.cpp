#include "gtest/gtest.h"
#include "../universal_reasoner/factsRepository.h"
#include <memory>
#include <string>
#include "../universal_reasoner/import.h"
#include "../universal_reasoner/metadata.h"

using namespace ureasoner;
using namespace std;

using COST = double;
TEST(BasicImport, ImportFact)
{
	importer::ImportedFact if1 { "OcenaCechOsobowych","ocena" };
	FactsRepository<COST, std::string> repo;
	auto s1 = FactFromImport<std::string>(if1);
	repo.AddFact(*s1,if1.name);
	auto  ressempty = repo.GetFactByName<std::string>("OcenaCechOsobowych");
	EXPECT_THROW(ressempty->GetValue(), std::logic_error);
}

TEST(BasicImport, ImportFactSmarterWay)
{
	importer::ImportedFact if1{ "OcenaCechOsobowych","ocena" };
	FactsRepository<COST, std::string> repo;
	AddToRepo(if1, repo);
	auto  ressempty = repo.GetFactByName<std::string>("OcenaCechOsobowych");
	EXPECT_THROW(ressempty->GetValue(), std::logic_error);
}

TEST(BasicImport, ImportRule)
{
	importer::ImportedFact if1{ "StanZdrowia","ZSD" };
	importer::ImportedFact if2{ "OcenaCechOsobowych","ocena" };
	auto repo = make_shared<FactsRepository<COST, std::string>>();
	Metadata<FactsRepository<COST, std::string>> data(repo);

 	shared_ptr < FactWrapperInterface < Metadata<FactsRepository<COST, std::string>>>> wrapperStan = MakeWrapper(data, if1);
 	auto premis1 = wrapperStan->MakePremise("zly");
// 
	shared_ptr < FactWrapperInterface < Metadata<FactsRepository<COST, std::string>>>> wrapperOcena = MakeWrapper(data, if2);
	auto conclusion1 = wrapperOcena->MakeConclusion("bardzo niska");

	RuleImpl<double> rule1(premis1, conclusion1);
 
	auto  ress = repo->GetFactByName<string>("StanZdrowia");
	EXPECT_THROW(ress->GetValue(), std::logic_error);


	ress->SetValue("zly");
	EXPECT_EQ(ress->GetValue(), "zly");


	EXPECT_TRUE(rule1.CheckAndFire());
	auto  resConclusion = repo->GetFactByName<string>("OcenaCechOsobowych");
	EXPECT_EQ(resConclusion->GetValue(), "bardzo niska");

}

TEST(BasicImport, ImportFromFile)
{
	auto facts = ureasoner::importer::ReadFactsFromRebitJSON("JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(facts.size(), 5);

	auto rules = ureasoner::importer::ReadRulesFromRebitJSON("JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(rules.size(), 18);

	auto repo = make_shared<FactsRepository<COST, std::string>>();
	Metadata<FactsRepository<COST, std::string>> data(repo);

	std::map<std::string, shared_ptr<FactWrapperInterface<decltype(data)>>> factsMap;

 	AddFacts(facts, factsMap, data);


	auto  ress = repo->GetFactByName<string>("OcenaCechOsobowych");
 	EXPECT_THROW(ress->GetValue(), std::logic_error);
	ress = repo->GetFactByName<string>("Plec");
	EXPECT_THROW(ress->GetValue(), std::logic_error);
	ress = repo->GetFactByName<string>("Wiek");
	EXPECT_THROW(ress->GetValue(), std::logic_error);
	ress = repo->GetFactByName<string>("StanZdrowia");
	EXPECT_THROW(ress->GetValue(), std::logic_error);
	ress = repo->GetFactByName<string>("StanCywilny");
	EXPECT_THROW(ress->GetValue(), std::logic_error);
	AddRules(rules, factsMap, data);
 
 	EXPECT_TRUE(true); //test checks is everything loads without an error
}