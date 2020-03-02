#include "gtest/gtest.h"
#include "../universal_reasoner/fact.h"
#include "../universal_reasoner/rule.h"
#include "../universal_reasoner/factsRepository.h"

using namespace ureasoner;
using COST = double;
TEST(BasicPremises, MakingPremise) 
{
	auto a1 = std::make_shared<FactConst<double>>(2.0);
	PremiseWithType<double> p(a1, 2.0);
	EXPECT_TRUE(p.Evaluate());
}

TEST(BasicPremises, MakingPremiseWithComparer)
{
	auto a1 = std::make_shared<FactConst<double>>(2.0);
	auto constComparer = [](const double& x, const double& y)->bool { return x < y; };
	auto comparer = [](double& x, double& y)->bool { return x < y; };

	PremiseWithType<double> p(a1, 3.0, 0.0, constComparer, comparer);
	EXPECT_TRUE(p.Evaluate());
}


TEST(BasicPremises, MakingPremiseFromRepo)
{
	FactsRepository<COST, double, int, std::string> repo;
	FactRepresentation<int> i1(1);
	FactRepresentation<std::string> s1("test");
	FactRepresentation<double> fempty;
	
	
	repo.AddFact(i1, "i1");
	repo.AddFact(s1, "s1");
	repo.AddFact(fempty, "fempty");

	auto resi1 = repo.GetFactByName<int>("i1")->GetValueShared();

	PremiseWithType<int> p(resi1, 1);
	EXPECT_TRUE(p.Evaluate());

	auto ress1 = repo.GetFactByName<std::string>("s1")->GetValueShared();

	PremiseWithType<std::string> p2(ress1, "test");
//	PremiseWithType<std::string> p2 = *MakePremise(ress1, (std::string)"test1");

	EXPECT_TRUE(p2.Evaluate());

	auto resd1 = repo.GetFactByName<double>("fempty")->GetValueShared();

	PremiseWithType<double> p3(resd1, 2.0);
	EXPECT_THROW(p3.Evaluate(), std::logic_error);
	resd1->SetValue(2.0);
	EXPECT_TRUE(p3.Evaluate());
}


TEST(FRepo, basicTest)
{
	FactsRepository<COST, double> a;
	FactsRepository<COST, double, int, std::string> b;
	FactsRepository<COST, int, short, double, long,bool> c;
// Command below, if uncommented, must result with compilation error since the list of types is not unique
//	FactsRepository<double, short, double, int, long, bool> d;

	FactRepresentation<double> f1(2.0);
	FactRepresentation<int> i1(1);
	FactRepresentation<bool> b1(true);
	FactRepresentation<std::string> s1("test");

	FactRepresentation<double> fempty;
	b.AddFact(fempty, "fempty");

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

	auto  resfempty = b.GetFactByName<double>("fempty");
	EXPECT_THROW(resfempty->GetValue(), std::logic_error);

	resfempty->SetValue(3.0);
	EXPECT_EQ(resfempty->GetValue(), 3.0);
}


TEST(FRepo, gettingAllKnownFacts)
{
	FactsRepository<COST, double> a;
	FactsRepository<COST, double, int, std::string> b;
	FactsRepository<COST, int, short, double, long, bool> c;
	
	FactRepresentation<double> f1(2.0);
	FactRepresentation<int> i1(1);
	FactRepresentation<bool> b1(true);
	FactRepresentation<std::string> s1("test");

	FactRepresentation<double> fempty;
	b.AddFact(fempty, "fempty");

	a.AddFact(f1, "f1");
	b.AddFact(s1, "s1");
	c.AddFact(i1, "i1");
	c.AddFact(f1, "f1");
	c.AddFact(b1, "b1");

	EXPECT_EQ(a.GetAllKnownFacts()->size(), 1);
	EXPECT_EQ(b.GetAllKnownFacts()->size(), 1);
	EXPECT_EQ(c.GetAllKnownFacts()->size(), 3);


	auto  resfempty = b.GetFactByName<double>("fempty");
	resfempty->SetValue(3.0);
	EXPECT_EQ(b.GetAllKnownFacts()->size(), 2);
}

TEST(BasicConclusions, MakingConclusion)
{
	auto f1 = std::make_shared<FactSettable<double>>();
	double d = 2.0;
	auto a1 = std::make_shared<ConclusionSettingFact<double>>(f1, d);
	EXPECT_THROW(f1->GetValue(), std::logic_error);
	a1->Execute();
	auto res = f1->GetValue();
	EXPECT_DOUBLE_EQ(res,2.0);
}

TEST(BasicRules, SimpleRule)
{
	auto f1 = std::make_shared<FactSettable<double>>();
	double d = 2.0;
	auto conclusion1 = std::make_shared<ConclusionSettingFact<double>>(f1, d);

	auto a1 = std::make_shared<FactConst<double>>(2.0);
	std::shared_ptr<Premise<double>> premis1 = std::make_shared<PremiseWithType<double>>(a1, 2.0);


	RuleImpl<double> rule1(premis1, conclusion1);

	EXPECT_THROW(f1->GetValue(), std::logic_error);
	EXPECT_TRUE(rule1.CheckAndFire());

	auto res = f1->GetValue();
	EXPECT_DOUBLE_EQ(res, 2.0);
}