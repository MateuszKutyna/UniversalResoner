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
	FactsRepository<double, int, std::string> b;
	FactsRepository<int, short, double, long,bool> c;
// Command below, if uncommented, must result with compilation error since the list of types is not unique
//	FactsRepository<double, short, double, int, long, bool> d;

	FactWithValue<double> f1(2.0);
	FactWithValue<int> i1(1);
	FactWithValue<bool> b1(true);
	FactWithValue<std::string> s1("test");
	a.AddFact(f1, "f1");
// Command below, if uncommented, must result with compilation error since there is no integer storage in repository a
//	a.AddFact(i1, "i1");
	b.AddFact(s1, "s1");
	c.AddFact(i1, "i1");
	c.AddFact(f1, "f1");
	c.AddFact(b1, "b1");

	auto  resf1 = c.GetFactByName<double>("f1");
	EXPECT_DOUBLE_EQ(resf1->GetValue(),2.0);

	resf1 = a.GetFactByName<double>("f1");
	EXPECT_DOUBLE_EQ(resf1->GetValue(), 2.0);

	auto  ress1 = b.GetFactByName<std::string>("s1");
	EXPECT_EQ(ress1->GetValue(), "test");

	auto  resi1 = c.GetFactByName<int>("i1");
	EXPECT_EQ(resi1->GetValue(), 1);

	auto  resb1 = c.GetFactByName<bool>("b1");
	EXPECT_EQ(resb1->GetValue(), true);

// Command below, if uncommented, must result with compilation error since there is no integer storage in repository a
//	auto  resi1Fail = a.GetFactByName<int>("i1");

	EXPECT_THROW(b.GetFactByName<int>("i1"),std::exception);
}