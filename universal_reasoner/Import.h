#ifndef Import_h__
#define Import_h__


#include "fact.h"
#include "rule.h"
#include <memory>
#include "../knowledge_importers/knowledge_importers.h"


namespace ureasoner
{
	// 	template<typename VALUE>
	// 	std::shared_ptr < Fact<VALUE>> FactFromImport(const importer::ImportedFact& importedFact)
	// 	{
	// 		return make_shared<FactSettable<VALUE>>();
	// 	}


	template <typename  REPO>
	void AddToRepo(const importer::ImportedFact& importedFact, REPO& repository)
	{
		importer::fillFact(repository, importedFact);
	}

	template <typename  REPO>
	std::map<std::string, std::string> AddFacts(std::vector<importer::ImportedFact>& facts, REPO& repository)
	{
		std::map<std::string, std::string> nameTypeMapper;
		for each (auto fact in facts)
		{
			importer::fillFact(repository, fact);
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
				make_shared<PremiseWithType<std::remove_cvref_t<T>>>
					(repo->GetFactByNameDynamic<std::remove_cvref_t<T>>(name), expectedValue)
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
			auto fact = repo->GetFactByNameDynamic<std::remove_cvref_t<T>>(name);
			cont->push_back(make_shared<ConclusionSettingFact<std::remove_cvref_t<T>>>(std::dynamic_pointer_cast<FactSettable<std::remove_cvref_t<T>>>(fact), expectedValue));	// remove cast if possible
		}
		;
	protected:
		std::shared_ptr<CONTAINER> cont;
		std::shared_ptr<REPO> repo;

	};

	template <typename  METADATA>
	void AddRules(std::vector<importer::ImportedRule>& rules, METADATA& data, std::map<std::string, std::string>& map)
	{
		using Premise = Premise<typename METADATA::CostType>;
		using Conclusion = Conclusion<typename METADATA::CostType>;
		std::shared_ptr< METADATA::FactsRepository> factsRepo = data.GetFactsRepository();

		for each (auto rule in rules)
		{
			auto premises = make_shared<vector<shared_ptr<Premise>>>();
			for each (auto premise in rule.premises)	// might be redesigned for template lambda!
			{
				auto factName = premise.factName;
				auto factType = map.find(factName)->second;

				PremiseInserter<Premise, vector<shared_ptr<Premise>>, METADATA::FactsRepository> premiseInserter(premises, factsRepo);
				importer::ConvertImportedTypes(premiseInserter, factName, factType, premise.expectedValue);
			}
			auto conclusions = make_shared<vector<shared_ptr<Conclusion>>>();
			for each (auto conclusion in rule.conclusions)
			{
				auto factName = conclusion.factName;
				auto factType = map.find(factName)->second;

				ConclusionInserter<Conclusion, vector<shared_ptr<Conclusion>>, METADATA::FactsRepository> conclusionInserter(conclusions, factsRepo);
				importer::ConvertImportedTypes(conclusionInserter, factName, factType, conclusion.valueToSet);
			}
			data.AddRule(make_shared<RuleImpl<double>>(*premises, *conclusions));
		}
	}
}
#endif // Import_h__
