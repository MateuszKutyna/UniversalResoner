#include "gtest/gtest.h"
#include "../knowledge_importers/knowledge_importers.h"

TEST(Importers, BasicRebitJsonImport)
{
	//Reads facts from JSON
	auto facts = ureasoner::importer::ReadFactsFromRebitJSON("../../JDuda.json", "CechyOsobowe_rules"); //copy JDuda to universal reasoner\x64\Debug4
	EXPECT_EQ(facts.size(), 5);

	//Reads rules from JSON
	auto rules = ureasoner::importer::ReadRulesFromRebitJSON("../../JDuda.json", "CechyOsobowe_rules"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(rules.size(), 18);
}

TEST(Importers, BasicRebitJsonImportFirstRulesSet)
{
	auto facts = ureasoner::importer::ReadFactsFromFirstRulesSetRebitJSON("../../JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(facts.size(), 5);

	auto rules = ureasoner::importer::ReadRulesFromFirstRulesSetRebitJSON("../../JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(rules.size(), 18);
}