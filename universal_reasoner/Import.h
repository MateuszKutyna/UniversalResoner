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
	public:;
		virtual std::shared_ptr<Premise> MakePremise(const string& expectedvalue) = 0;
		virtual std::shared_ptr<Conclusion> MakeConclusion(const string& settabledvalue) = 0;
	};

	template <typename METADATA, typename T>
	class FactWrapper : public FactWrapperInterface<METADATA>
	{
	public:


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
			return make_shared < FactWrapper < Metadata<FactsRepository<std::string>>, std::string>>(metadata, importedFact);
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
}
#endif // Import_h__
