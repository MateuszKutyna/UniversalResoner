#include "gtest/gtest.h"
#include "../universal_reasoner/fact.h"
#include "../universal_reasoner/rule.h"
#include "../universal_reasoner/factsRepository.h"
#include <memory>
using namespace ureasoner;
using COST = double;
TEST(BasicPremises, MakingPremise) 
{
	
	auto a1 = std::make_shared < FactConst < double >> (2.0);
	PremiseWithType < double > p(a1, 2.0);
	//checkes if values are equal in this case 2.0==2.0 is true
	EXPECT_TRUE(p.Evaluate());
}

TEST(BasicPremises, MakingPremiseWithComparer)
{
	auto a1 = std::make_shared<FactConst<double>>(2.0);
	auto constComparer = [](const double& x, const double& y)->bool { return x < y; };

	//Does what above but with custom Comparer
	PremiseWithType<double> p(a1, 3.0, 0.0, constComparer);
	EXPECT_TRUE(p.Evaluate());
}

TEST(BasicPremises, MakingPremiseFromRepo)
{
	FactsRepository<COST, double, int, std::string> repo;
	//fact<int>(1) fact<string>(test) fact<double>(empty)
	auto i1 = std::make_shared<FactConst<int>>(1);
	auto s1 = std::make_shared<FactConst<std::string>>("test");

	//Empty but it can be
	auto fempty = std::make_shared<FactSettable<double>>();
	
	//Adding facts with a name
	repo.AddFact(i1, "i1");
	repo.AddFact(s1, "s1");
	repo.AddFact(fempty, "fempty");

	//taking fact from un_orderedmap
	auto resi1 = repo.GetFactByName<int>("i1");

	//Comparing it 
	PremiseWithType<int> p(resi1, 1);
	EXPECT_TRUE(p.Evaluate());

	//taking fact from un_orderedmap
	auto ress1 = repo.GetFactByName<std::string>("s1");

	//Comparing it 
	PremiseWithType<std::string> p2(ress1, "test");
	EXPECT_TRUE(p2.Evaluate());

	auto resd1 = repo.GetFactByName<double>("fempty");

	//empty so logic_error
	PremiseWithType<double> p3(resd1, 2.0);
	EXPECT_THROW(p3.Evaluate(), std::logic_error);
	//setting facts value to 2.0 and then compering
	resd1->SetValue(2.0);
	EXPECT_TRUE(p3.Evaluate());
}

TEST(FRepo, basicTest)
{
	/*
	In this test, heterogeneous repositories are tested. FactsRepository is a structure, able to store values with different types, but only ones defined as template parameters.
	The aim is to keep control of allowed types - a designer can configure FactsRepository specialization for a particular reasoning engine
	All items stored in repository must share the same type of Cost (of evaluation a value), given by the first template parameter, COST
	*/
	FactsRepository<COST, double> a;
	FactsRepository<COST, double, int, std::string> b;
	FactsRepositoryDouble<int, short, double, long,bool> c; //template alias with COST = double specialization
																
// Command below, if uncommented, must result with compilation error since the list of types is not unique		
//	FactsRepository<COST, double, short, double, int, long, bool> d;

	auto i1 = std::make_shared<FactConst<int>>(1);
	auto f1 = std::make_shared<FactConst<double>>(2.0);
	auto b1 = std::make_shared<FactConst<bool>>(true);
	auto s1 = std::make_shared<FactConst<std::string>>("test");
	auto fempty = std::make_shared<FactSettable<double>>();


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
	
	auto i1 = std::make_shared<FactConst<int>>(1);
	auto f1 = std::make_shared<FactConst<double>>(2.0);
	auto b1 = std::make_shared<FactConst<bool>>(true);
	auto s1 = std::make_shared<FactConst<std::string>>("test");
	auto fempty = std::make_shared<FactSettable<double>>();

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

	//Value isn't being set yet 
	//We must call Execute first to set this value
	auto a1 = std::make_shared<ConclusionSettingFact<double>>(f1, d);
	EXPECT_THROW(f1->GetValue(), std::logic_error);

	a1->Execute();
	//Execute has been called so the value is set
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
	//Not set so logic error
	EXPECT_THROW(f1->GetValue(), std::logic_error);

	//Basicly sets fact 
	EXPECT_TRUE(rule1.CheckAndFire());

	//Because checkAndFire is setting value of the fact we can get it's value
	auto res = f1->GetValue();
	EXPECT_DOUBLE_EQ(res, 2.0);
}