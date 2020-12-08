
#include <iostream>
#include <factsRepository.h>
#include <import.h>
#include <metadata.h>
#include <memory>
#include <string>
#include<type_traits>
#include<concepts>
using namespace ureasoner;
using namespace std;

using COST = double;
int main()
{
	importer::ImportedFact if1{ "OcenaCechOsobowych","ocena" };
	FactsRepository<COST, std::string> repo;
	AddToRepo(if1, repo);
	auto  ressempty = repo.GetFactByName<std::string>("OcenaCechOsobowych");
}

