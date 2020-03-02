#ifndef Import_h__
#define Import_h__

#include "../knowledge_importers/knowledge_importers.h"
#include "fact.h"
#include "rule.h"
#include <memory>

namespace ureasoner
{

	template<typename METADATA>
	class FactWrapperInterface
	{
	public:
		virtual std::shared_ptr<Premise<typename METADATA::CostType>> MakePremise(const string& expectedvalue) = 0;
		virtual std::shared_ptr<Conclusion<typename METADATA::CostType>> MakeConclusion(const string& settabledvalue) = 0;
	};

	template <typename METADATA, typename T>
	class FactWrapper : public FactWrapperInterface<METADATA>
	{
	public:
		using CostType = typename METADATA::CostType;
		using Premise = Premise<CostType>;
		using Conclusion = Conclusion<CostType>;

		FactWrapper(METADATA& metadata, const importer::ImportedFact& importedFact) : fact(metadata.AddFact(FactRepresentation<T>(), importedFact.name)) {}


		virtual std::shared_ptr<Premise> MakePremise(const string& expectedvalue) override
		{
			return make_shared<PremiseWithType<T>>(fact->GetValueShared(), (T)expectedvalue);
		}

		virtual std::shared_ptr<Conclusion> MakeConclusion(const string& settabledvalue) override
		{
			return make_shared<ConclusionSettingFact<T>>(std::dynamic_pointer_cast<FactSettable<T>>(fact->GetValueShared()), (T)settabledvalue);
		}
	protected:
		const std::shared_ptr< FactRepresentation<T>> fact;
	};

	constexpr unsigned int str2int(const char* str, int h = 0)
	{
		return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
	}

	template<typename METADATA>
	auto MakeWrapper(METADATA& metadata, const importer::ImportedFact& importedFact)
	{
		string typeName = importedFact.type;
		switch (str2int(typeName.c_str()))
		{
		case str2int("ocena"):
		case str2int("WZ"):
		case str2int("ZSD"):
		case str2int("wiek"):
		case str2int("KM"):
			return make_shared < FactWrapper < Metadata<FactsRepository<COST, std::string>>, std::string>>(metadata, importedFact);
			break;
		default:
			throw std::logic_error("AddToRepo tried to create a variable of type not registered in repository");
			break;
		}
	};


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

	template <typename METADATA>
	void AddFacts(vector<importer::ImportedFact>& facts, std::map<std::string, shared_ptr<FactWrapperInterface<METADATA>>>& factsMap, METADATA& data)
	{
		for each (auto fact in facts)
		{
			//factsMap.insert(std::pair<std::string, shared_ptr<FactWrapperInterface<METADATA>>>(fact.name, MakeWrapper(data, fact)));
			shared_ptr<FactWrapperInterface<METADATA>>  wrapped = MakeWrapper(data, fact);
			std::pair< std::string, shared_ptr<FactWrapperInterface<METADATA>> > pr = std::make_pair(fact.name, wrapped);
			factsMap.insert(pr);
		}
	}



	template <typename METADATA>
	void AddRules(vector<importer::ImportedRule>& rules, std::map<std::string, shared_ptr<FactWrapperInterface<METADATA>>> factsMap, METADATA& data)
	{
		using Premise = Premise<typename METADATA::CostType>;
		using Conclusion = Conclusion<typename METADATA::CostType>;

		for each (auto rule in rules)
		{
			vector<shared_ptr<Premise>> premises;
			for each (auto premise in rule.premises)
			{
				auto factName = premise.factName;
				auto res = factsMap.find(factName);
				premises.push_back(res->second->MakePremise(premise.expectedValue));
			}
			vector<shared_ptr<Conclusion>> conclusions;
			for each (auto conclusion in rule.conclusions)
			{
				auto factName = conclusion.factName;
				auto res = factsMap.find(factName);
				conclusions.push_back(res->second->MakeConclusion(conclusion.valueToSet));
			}
			data.AddRule(make_shared<RuleImpl<double>>(premises, conclusions));
		}
	}

}
#endif // Import_h__
