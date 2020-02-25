#include "gtest/gtest.h"
#include "../knowledge_importers/knowledge_importers.h"
#include <fstream>      // std::ifstream

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(Importers, BasicRebitJsonImport)
{
	std::ifstream ifs("a.txt", std::ifstream::in);

	char c = ifs.get();

	std::ifstream i("a");
	auto facts = ureasoner::importer::ReadFacts("JDuda.json");
	EXPECT_EQ(facts.size(), 18);
}