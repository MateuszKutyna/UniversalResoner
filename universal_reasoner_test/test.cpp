#include "gtest/gtest.h"
#include "../universal_reasoner/fact.h"
#include "../universal_reasoner/rule.h"
#include "../universal_reasoner/factsRepository.h"

using namespace ureasoner;
TEST(BasicPremises, MakingPremise) 
{
	auto a1 = std::make_shared<FactWithValue<double>>(2.0);
	FactWithValue<double> a2(2.0);
	PremiseWithType<double> p(a1, a2);
	EXPECT_TRUE(p.Evaluate());
}

TEST(BasicPremises, MakingPremiseWithVirtual)
{
	auto a1 = std::make_shared<FactWithValue<double>>(2.0);
	FactWithValue<double> a2(2.0);
	std::shared_ptr<Premise> p = std::make_shared<PremiseWithType<double>>(a1, a2);
	EXPECT_TRUE(p->Evaluate());
}

TEST(FRepo, basicTest)
{
	FactsRepository<double> a;
	FactsRepository<double, int> b;
//	FactsRepository<double, int, bool> b;
	FactsRepository<int, short, double, long,bool> c;
//	FactsRepository<double, short, double, int, long, bool> d;

	FactWithValue<double> f1(2.0);
	FactWithValue<int> i1(1);
	FactWithValue<bool> b1(true);
	a.AddFact(f1, "f1");
//	a.AddFact(i1, "i1");
	c.AddFact(i1, "i1");
	c.AddFact(f1, "f1");
	c.AddFact(b1, "b1");

	auto  res = c.GetFactByName<double>("f1");
	EXPECT_DOUBLE_EQ(res->GetValue(),2.0);
/*	EXPECT_TRUE(true);*/
}