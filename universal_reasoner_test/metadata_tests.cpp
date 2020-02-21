#include "gtest/gtest.h"
#include "../universal_reasoner/metadata.h"
#include "../universal_reasoner/factsRepository.h"
#include <memory>

using namespace ureasoner;
using namespace std;
TEST(BasicMetadata, ForwardReasoning)
{
	// Add Facts
	// Add Rules
	// Evaluate all possible facts
		// 1. find all valued facts
		// 2. find first rule to be fired
		// 3. if true goto 1 else goto 2
	EXPECT_TRUE(true);
}

TEST(BasicMetadata, AddingFacts)
{
	auto repo = make_shared<FactsRepository<std::string>>();
	Metadata<FactsRepository<std::string>> data(repo);

	repo->AddFact(FactWithValue < std::string>("test1"), "test1");
	repo->AddFact("test2", "test2");
	repo->AddFact(FactWithValue<string>(),"test3");

	auto  ress1 = repo->GetFactByName<std::string>("test1");
	EXPECT_EQ(ress1->GetValue(), "test1");

	auto  ress2 = repo->GetFactByName<string>("test3");
	EXPECT_THROW(ress2->GetValue(), std::logic_error);
}

TEST(BasicMetadata, AddingMoreComplexFacts)
{
	auto repo = make_shared<FactsRepository<string, int, double>>();
	Metadata<decltype(repo)::element_type> data(repo);

	repo->AddFact(FactWithValue < std::string>("test1"), "test1");
	repo->AddFact("test2", "test2");
	repo->AddFact(FactWithValue<string>(), "test3");
	repo->AddFact(2.0, "d1");
	repo->AddFact(2, "i1");

	auto  ress1 = repo->GetFactByName<std::string>("test1");
	EXPECT_EQ(ress1->GetValue(), "test1");

	auto  resi1 = repo->GetFactByName<int>("i1");
	EXPECT_EQ(resi1->GetValue(), 2);

	auto  ress2 = repo->GetFactByName<string>("test3");
	EXPECT_THROW(ress2->GetValue(), std::logic_error);
}
TEST(BasicMetadata, BackwardReasoning)
{
	// Add Facts
	// Add Rules
	// Ask for fact
		// Find rule providing fact
		// get all necessary facts
		// ask for all necessary uknown facts
	
	EXPECT_TRUE(true);
}