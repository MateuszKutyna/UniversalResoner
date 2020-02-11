#include "gtest/gtest.h"
#include "../universal_reasoner/fact.h"
#include "../universal_reasoner/rule.h"

using namespace ureasoner;
TEST(BasicPremises, MakingPremise) 
{
	auto a1 = std::make_shared<FactWithValue<double>>(2.0);
	FactWithValue<double> a2(2.0);
	PremiseWithType<double> p(a1, a2, [](const double& x, const double& y)->bool { return x == y; });
	EXPECT_TRUE(p.Evaluate());
}

TEST(BasicPremises, MakingPremiseWithVirtual)
{
	auto a1 = std::make_shared<FactWithValue<double>>(2.0);
	FactWithValue<double> a2(2.0);
	std::shared_ptr<Premise> p = std::make_shared<PremiseWithType<double>>(a1, a2, [](const double& x, const double& y)->bool { return x == y; });
	EXPECT_TRUE(p->Evaluate());
}