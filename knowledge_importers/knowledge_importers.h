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
		struct EmptyVar { using Type = T; };

		struct ImportedFact
		{
			ImportedFact(string factName, string factType) noexcept : name(factName), type(factType) {}
			string name;
			string type;
		};

// 		template<typename A, typename B, typename C>
// 		void Inserter(A& map, B& data, C& fact)
// 		{
// 			string typeName = fact.type;
// 			switch (str2int(typeName.c_str()))
// 			{
// 				case str2int("Real"):
// 				{
// 					auto fw = FactWrapper(data, importer::EmptyVar<float>(), fact);
// 					auto fws = make_shared < decltype(fw)>(std::move(fw));
// 					map.insert(std::pair(fact.name, MakeWrapper(data, fact)));
// 				}
// 				default:
// 				{
// 					auto fw = FactWrapper(data, importer::EmptyVar<std::string>(), fact);
// 					auto fws = make_shared < decltype(fw)>(std::move(fw));
// 					map.insert(std::pair(fact.name, MakeWrapper(data, fact)));
// 				}
// 				}
// 		}


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
			case str2int("Real"):
			{
				repo.AddFact(EmptyVar<float>(), fact.name);
				break;
			}
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

		template<typename INSERTER>
		void fillPremise(INSERTER& premiseInserter, const string& factName, const string& factType, const string& expectedValue)
		{
			switch (str2int(factType.c_str()))
			{
			case str2int("Real"):
			{
//				premiseInserter.Insert(factName, expectedValue);
 				auto val = std::stof(expectedValue);
 				premiseInserter.Insert(factName, val);
			}
			default:
			{
				premiseInserter.Insert(factName, expectedValue);
			}
			}
		}

		template<typename INSERTER>
		void fillConclusion(INSERTER& conclusionInserter, const string& factName, const string& factType, const string& setsValue)
		{
			switch (str2int(factType.c_str()))
			{
			case str2int("Real"):
			{
				auto val = std::stof(setsValue);
				conclusionInserter.Insert(factName, val);
//				conclusionInserter.Insert(factName, setsValue);
			}
			default:
			{
				conclusionInserter.Insert(factName, setsValue);
			}
			}
		}

// 		template<typename CONTAINER, typename REPO>
// 		void fillPremise(CONTAINER& premises, REPO& factsRepo,const std::string& typeName, const std::string& factName, const std::string& expectedValue)
// 		{
// 			
// 			switch (str2int(typeName.c_str()))
// 			{
// 			case str2int("Real"):
// 			{
// 				auto res = factsRepo.GetFactByName<std::string>(factName); //////////////////////!
// //				premises.push_back(res->second->MakePremise(expectedValue));
// 
// // 				virtual std::shared_ptr<Premise> MakePremise(const string & expectedvalue) override
// // 				{
// // 					return make_shared<PremiseWithType<T>>(fact->GetValueShared(), (T)expectedvalue);
// // 				}
// 
// 			}
// 			default:
// 			{
// 				auto res = factsRepo.GetFactByName<std::string>(factName);
// //				premises.push_back(res->second->MakePremise(expectedValue));
// 			}
// 			}
// 		}


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

