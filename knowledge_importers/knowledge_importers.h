#ifndef knowledgeImporters_knowledgeImporters_h__
#define knowledgeImporters_knowledgeImporters_h__

#include <string>
#include <vector>

namespace ureasoner
{
	namespace importer
	{
		constexpr unsigned int str2int(const char* str, int h = 0)
		{
			return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
		}

		template<typename T>
		struct EmptyVar { using Type = T; };

		struct ImportedFact
		{
			ImportedFact(const std::string& factName, const std::string& factType) : name(factName), type(factType) {}
			const std::string name;
			const std::string type;
		};

		struct ImportedPremise
		{
			const std::string factName;
			const std::string expectedValue;
		};

		struct ImportedConclusion
		{
			const std::string factName;
			const std::string valueToSet;
		};

		struct ImportedRule
		{
			std::vector<ImportedPremise> premises;
			std::vector<ImportedConclusion> conclusions;
		};

		std::vector<ImportedFact> ReadFactsFromFirstRulesSetRebitJSON(const std::string& filename);
		std::vector<ImportedFact> ReadFactsFromRebitJSON(const std::string& filename, const std::string& rulesetName);
		std::vector<ImportedRule> ReadRulesFromFirstRulesSetRebitJSON(const std::string& filename);
		std::vector<ImportedRule> ReadRulesFromRebitJSON(const std::string& filename, const std::string& rulesetName);

		template<typename T>
		void fillFact(T& repo, const ImportedFact& fact)
		{
			switch (str2int(fact.type.c_str()))
			{
			case str2int("Real"):
			{
				repo.AddFact(EmptyVar<float>(), fact.name);
				break;
			}
			default:
				repo.AddFact(EmptyVar<std::string>(), fact.name);
			}
		}

		template<typename INSERTER>
		void ConvertImportedTypes(INSERTER& inserter, const std::string& factName, const std::string& factType, const std::string& setsValue)
		{
			switch (str2int(factType.c_str()))
			{
			case str2int("Real"):
			{
				inserter.Insert(factName, std::stof(setsValue));
			}
			default:
			{
				inserter.Insert(factName, setsValue);
			}
			}
		}
	}
}

#endif // knowledgeImporters_knowledgeImporters_h__

