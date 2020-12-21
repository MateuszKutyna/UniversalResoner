#ifndef Import_h__
#define Import_h__


#include "fact.h"
#include "rule.h"
#include <memory>
#include "../knowledge_importers/knowledge_importers.h"
#include<map>
#include <type_traits>

namespace ureasoner
{
	template<typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	template <typename  REPO>
	void AddToRepo(const importer::ImportedFact& importedFact, REPO& repository)
	{
		importer::fillFact(repository, importedFact);
	}

	template <typename  REPO>
	std::map<std::string, std::string> AddFacts(const std::vector<importer::ImportedFact>& facts, REPO& repository)
	{

		std::map<std::string, std::string> nameTypeMapper;
		for(const auto& fact: facts)
		{
			//Inserts facts into repository
			importer::fillFact(repository, fact);
			//Inserts facts into map
			nameTypeMapper.insert(std::pair(fact.name, fact.type));
		}
		return std::move(nameTypeMapper);
	}

	template<typename PREMISE, typename CONTAINER, typename REPO>
	class PremiseInserter
	{
	public:
		PremiseInserter(std::shared_ptr<CONTAINER> cont, std::shared_ptr<REPO> repo) : cont(cont), repo(repo) {}

		template <typename T>
		void Insert(const std::string& name, T&& expectedValue)
		{
			cont->push_back(
				std::make_shared<PremiseWithType<remove_cvref_t<T>>>(repo->GetFactByNameDynamic<remove_cvref_t<T>>(name), expectedValue)
			);

		};
	protected:
		std::shared_ptr<CONTAINER> cont;
		std::shared_ptr<REPO> repo;
	};

	template<typename CONCLUSION, typename CONTAINER, typename REPO>
	class ConclusionInserter
	{
	public:
		ConclusionInserter(std::shared_ptr<CONTAINER> cont, std::shared_ptr<REPO> repo) : cont(cont), repo(repo) {}

		template <typename T>
		void Insert(const std::string& name, T&& expectedValue)
		{
			auto fact = repo->GetFactByNameDynamic<remove_cvref_t<T>>(name);
			cont->push_back(std::make_shared<ConclusionSettingFact<remove_cvref_t<T>>>(std::dynamic_pointer_cast<FactSettable<remove_cvref_t<T>>>(fact), expectedValue));	// remove cast if possible
		};

	protected:
		std::shared_ptr<CONTAINER> cont;
		std::shared_ptr<REPO> repo;

	};

	template <typename  METADATA>
	void AddRules(std::vector<importer::ImportedRule>& rules, METADATA& data, std::map<std::string, std::string>& map)
	{
		using Premise = Premise<METADATA::CostType>;
		using Conclusion = Conclusion<METADATA::CostType>;
		std::shared_ptr<METADATA::FactsRepository> factsRepo = data.GetFactsRepository();
		//Wyci¹ga regu³y z importowanych i dodaje je w postaci ImportedRule
		//POTENCJALNE MIEJSCE NA ZROWNOLEGLENIE 
		for (const auto& rule: rules)
		{
			//Tymczasowy premise
			auto premises = std::make_shared<std::vector<std::shared_ptr<Premise>>>();
			PremiseInserter<Premise, std::vector<std::shared_ptr<Premise>>, METADATA::FactsRepository> premiseInserter(premises, factsRepo);
			for (const auto& premise: rule.premises)
			{
				const auto factName = premise.factName;
				importer::ConvertImportedTypes(premiseInserter, factName, map.find(factName)->second, premise.expectedValue);
			}

			//Tymczasowa konkluzja
			auto conclusions = std::make_shared < std:: vector < std::shared_ptr<Conclusion >> > ();
			ConclusionInserter<Conclusion, std::vector<std::shared_ptr<Conclusion>>, METADATA::FactsRepository> conclusionInserter(conclusions, factsRepo);
			for (const auto& conclusion: rule.conclusions)
			{
				const auto factName = conclusion.factName;
				importer::ConvertImportedTypes(conclusionInserter, factName, map.find(factName)->second, conclusion.valueToSet);
			}
			
			data.AddRule(std::make_shared<RuleImpl<double>>(*premises, *conclusions));
		}
	}
}
#endif // Import_h__
