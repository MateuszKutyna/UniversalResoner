#include "gtest/gtest.h"
#include "../knowledge_importers/knowledge_importers.h"

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(Importers, BasicRebitJsonImport)
{
	auto facts = ureasoner::importer::ReadFacts("JDuda.json"); //copy JDuda to universal reasoner\x64\Debug
	EXPECT_EQ(facts.size(), 18);
}