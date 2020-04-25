#include "gtest/gtest.h"
#include "../knowledge_importers/knowledge_importers.h"

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(Importers, BasicRebitJsonImport)
{
	auto facts = ureasoner::importer::ReadFactsFromRebitJSON("JDuda.json", "CechyOsobowe_rules"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(facts.size(), 5);

	auto rules = ureasoner::importer::ReadRulesFromRebitJSON("JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(rules.size(), 18);
}

TEST(Importers, BasicRebitJsonImportFirstRulesSet)
{
	auto facts = ureasoner::importer::ReadFactsFromFirstRulesSetRebitJSON("JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(facts.size(), 5);

	auto rules = ureasoner::importer::ReadRulesFromRebitJSON("JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(rules.size(), 18);
}