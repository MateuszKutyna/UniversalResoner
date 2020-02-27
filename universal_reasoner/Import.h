#ifndef Import_h__
#define Import_h__

#include "../knowledge_importers/knowledge_importers.h"
#include "fact.h"
#include "rule.h"
#include <memory>

namespace ureasoner
{
	constexpr unsigned int str2int(const char* str, int h = 0)
	{
		return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
	}


	template<typename VALUE, template <typename> typename FACT_CONST = FactConst, template <typename> typename FACT_SETTABLE = FactSettable>
	std::shared_ptr < FactRepresentation<VALUE, FACT_CONST, FACT_SETTABLE>> FactFromImport(const importer::ImportedFact& importedFact)
	{
		return make_shared< FactRepresentation<VALUE, FACT_CONST, FACT_SETTABLE>>();
	}


	template <typename  REPO>
	void AddToRepo(const importer::ImportedFact& importedFact, REPO& repository)
	{
		string typeName = importedFact.type;
		switch (str2int(typeName.c_str()))
		{
		case str2int("ocena"):
		case str2int("WZ"):
		case str2int("ZSD"):
		case str2int("wiek"):
		case str2int("KM"):
			repository.AddFact(*FactFromImport<std::string>(importedFact), importedFact.name);
			break;
		default:
			throw std::logic_error("AddToRepo tried to create a variable of type not registered in repository");
			break;
		}
	}



	// 	template <typename REPOSITORY, typename COST = double>
	// 	shared_ptr<Rule<COST>> RuleFromImport(const importer::ImportedRule& importedRule, std::vector<importer::ImportedFact> facts, REPOSITORY& repo)
	// 	{
	// 		auto resi1 = repo.GetFactByName<int>("i1")->GetValueShared();
	// 		auto conclusion1 = std::make_shared<ConclusionSettingFact<double>>(f1, d);
	// 
	// 		auto a1 = std::make_shared<FactConst<double>>(2.0);
	// 		std::shared_ptr<Premise> premis1 = std::make_shared<PremiseWithType<double>>(a1, 2.0);
	// 
	// 
	// 		RuleImpl<double> rule1(premis1, conclusion1);
	// 	}
}
#endif // Import_h__
