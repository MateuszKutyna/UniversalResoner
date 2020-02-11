#include "gtest/gtest.h"
#include "../universal_reasoner/fact.h"
#include "../universal_reasoner/rule.h"

using namespace ureasoner;
TEST(BasicFacts, MakingFact) 
{
	auto a1 = std::make_shared<FactWithValue<double>>(2.0);
	FactWithValue<double> a2(2.0);
	PremiseWithType<double> p(a1, a2);
	EXPECT_TRUE(p.Evaluate());
}