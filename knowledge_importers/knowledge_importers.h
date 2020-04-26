#ifndef knowledgeImporters_knowledgeImporters_h__
#define knowledgeImporters_knowledgeImporters_h__

#include <vector>
#include <string>
#include <array>


using std::string;
using std::vector;

namespace ureasoner
{
	namespace importer
	{
		constexpr unsigned int str2int(const char* str, int h = 0)
		{
			return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
		}


		template<unsigned int TypeNameEncoded>
		struct TypeName2Type
		{
			using TypeName = std::string;
		};

		template<>
		struct TypeName2Type<str2int("Real")>
		{
			using TypeName = float;
		};

		template<typename T>
		struct EmptyVar {};

		struct ImportedFact
		{
			ImportedFact(string factName, string factType) noexcept : name(factName), type(factType) {}
			string name;
			string type;
		};

// 		template<typename WRAPPER>
// 		void FillWrapper(WRAPPER& metadata, const ImportedFact& fact)
// 		{
// 			string typeName = fact.type;
// 			switch (str2int(typeName.c_str()))
// 			{
// 			case str2int("Real"):
// 			{
// 				auto fw = FactWrapper(metadata, importer::EmptyVar<float>(), fact);
// 				factsMap.insert(std::pair(fact.name, MakeWrapper(data, fact)));
// 				make_shared < decltype(fw)>(std::move(fw));
// 			}
// 			default:
// 			{
// 				auto fw = FactWrapper(metadata, importer::EmptyVar<std::string>(), fact);
// 				return make_shared < decltype(fw)>(std::move(fw));
// 			}
// 			}
// 		}

		template<typename T>
		void fillFact(T& repo, const ImportedFact& fact)
		{
			string typeName = fact.type;
			switch (str2int(typeName.c_str()))
			{
			case str2int("ocena"):
			case str2int("WZ"):
			case str2int("ZSD"):
			case str2int("wiek"):
			case str2int("KM"):
				repo.AddFact(EmptyVar<std::string>(), fact.name);
				break;
			default:
				throw std::logic_error("AddToRepo tried to create a variable of type not registered in repository");
				break;
			}
		}

		struct ImportedPremise
		{
			string factName;
			string expectedValue;
		};

		struct ImportedConclusion
		{
			string factName;
			string valueToSet;
		};

		struct ImportedRule
		{
			vector<ImportedPremise> premises;
			vector<ImportedConclusion> conclusions;
		};

		vector<ImportedFact> ReadFactsFromFirstRulesSetRebitJSON(const std::string& filename);
		vector<ImportedFact> ReadFactsFromRebitJSON(const std::string& filename, const std::string& rulesetName);
		vector<ImportedRule> ReadRulesFromFirstRulesSetRebitJSON(const std::string& filename);
		vector<ImportedRule> ReadRulesFromRebitJSON(const std::string& filename, const std::string& rulesetName);


	}
}

#endif // knowledgeImporters_knowledgeImporters_h__

