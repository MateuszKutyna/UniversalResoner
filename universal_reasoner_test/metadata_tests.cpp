#include "gtest/gtest.h"
#include "../universal_reasoner/metadata.h"
#include "../universal_reasoner/factsRepository.h"
#include <memory>
#include "../universal_reasoner/rule.h"

using namespace ureasoner;
using namespace std;
using COST = double;

TEST(BasicMetadata, AddingFacts)
{

	auto repo = make_shared<FactsRepository<COST, std::string>>();
	Metadata<FactsRepository<COST, std::string>> data(repo);

	repo->AddFact(std::make_shared<FactConst<string>>("test1"), "test1");
	repo->AddFact("test2", "test2");
	repo->AddFact(FactSettable<string>(),"test3");

	auto  ress1 = repo->GetFactByName<std::string>("test1");
	EXPECT_EQ(ress1->GetValue(), "test1");

	auto  ress2 = repo->GetFactByName<string>("test3");
	EXPECT_THROW(ress2->GetValue(), std::logic_error);
}

TEST(BasicMetadata, AddingMoreComplexFacts)
{
	auto repo = make_shared<FactsRepository<COST, string, int, double>>();
	Metadata<decltype(repo)::element_type> data(repo);

	repo->AddFact(std::make_shared<FactConst<string>>("test1"), "test1");
	repo->AddFact("test2", "test2");
	repo->AddFact(FactSettable<string>(), "test3");
	repo->AddFact(2.0, "d1");
	repo->AddFact(2, "i1");

	auto  ress1 = repo->GetFactByName<std::string>("test1");
	EXPECT_EQ(ress1->GetValue(), "test1");

	auto  resi1 = repo->GetFactByName<int>("i1");
	EXPECT_EQ(resi1->GetValue(), 2);

	auto  ress2 = repo->GetFactByName<string>("test3");
	EXPECT_THROW(ress2->GetValue(), std::logic_error);
}

TEST(BasicMetadata, AddingRules)
{

	auto repo = make_shared<FactsRepository<COST, string, int, double>>();
	Metadata<decltype(repo)::element_type> data(repo);

	repo->AddFact(std::make_shared<FactConst<string>>("test1"), "test1");
	repo->AddFact("test2", "test2");

	
	repo->AddFact(FactSettable<string>(), "test3");
 	auto s3 = repo->GetSettableFactByName<string>("test3");
	repo->AddFact(2.0, "d1");
	repo->AddFact(2, "i1");


	auto conclusion1 = std::make_shared<ConclusionSettingFact<string>>(s3, "test3");


 	std::shared_ptr<Premise<double>> premis1 = std::make_shared<PremiseWithType<string>>(repo->GetFactByName<std::string>("test1"), "test1");

	auto  ress3 = repo->GetFactByName<string>("test3");
	EXPECT_THROW(ress3->GetValue(), std::logic_error);
	auto rule1 = make_shared<RuleImpl<double>>(premis1, conclusion1);
	data.AddRule(rule1);
 	EXPECT_TRUE(rule1->CheckAndFire());
 	EXPECT_EQ(ress3->GetValue(), "test3");
}
