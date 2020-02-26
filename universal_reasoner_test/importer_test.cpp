#include "gtest/gtest.h"
#include "../universal_reasoner/factsRepository.h"
#include <memory>
#include <string>
#include "../universal_reasoner/import.h"

using namespace ureasoner;
using namespace std;
TEST(BasicImport, ImportFact)
{
	importer::ImportedFact if1 { "OcenaCechOsobowych","ocena" };
	FactsRepository<std::string> repo;
	auto s1 = FactFromImport<std::string>(if1);
	repo.AddFact(*s1,if1.name);
	auto  ressempty = repo.GetFactByName<std::string>("OcenaCechOsobowych");
	EXPECT_THROW(ressempty->GetValue(), std::logic_error);
}
