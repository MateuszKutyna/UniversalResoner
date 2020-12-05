#include "gtest/gtest.h"
#include "../universal_reasoner/factsRepository.h"
#include <memory>
#include <string>
#include "../universal_reasoner/import.h"
#include "../universal_reasoner/metadata.h"

using namespace ureasoner;
using namespace std;

using COST = double;

TEST(BasicImport, ImportFactSmarterWay)
{
	importer::ImportedFact if1{ "OcenaCechOsobowych","ocena" };
	FactsRepository<COST, std::string> repo;
	AddToRepo(if1, repo);
	auto  ressempty = repo.GetFactByName<std::string>("OcenaCechOsobowych");

	//It can't get it's value because it's not FactSettable
	EXPECT_THROW(ressempty->GetValue(), std::logic_error);
}