#ifndef Import_h__
#define Import_h__

#include "../knowledge_importers/knowledge_importers.h"
#include "fact.h"

namespace ureasoner
{
	template<typename VALUE, template <typename> typename FACT_CONST = FactConst, template <typename> typename FACT_SETTABLE = FactSettable>
	std::shared_ptr < FactRepresentation<VALUE, FACT_CONST, FACT_SETTABLE>> FactFromImport(const importer::ImportedFact & importedFact)
	{
		return make_shared< FactRepresentation<VALUE, FACT_CONST, FACT_SETTABLE>>();
	}
}
#endif // Import_h__
