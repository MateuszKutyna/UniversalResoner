#ifndef universal_reasoner_metadata_h__
#define universal_reasoner_metadata_h__

#include "fact.h"
#include "executableWithCost.h"
#include <unordered_map>
#include <unordered_set>

namespace ureasoner
{
	template<typename EXECUTABLE>
	class Metadata
	{
	public:
		typedef EXECUTABLE ExecutableWithCost;
		std::unordered_map<Fact, bool> GetAllFacts();
		std::unordered_set<Fact> GetKnownFacts();
		std::vector<ExecutableWithCost> GetExecutablesProvidingFact(const Fact& fact);
		std::vector<ExecutableWithCost> GetExecutablesUsingFact(const Fact& fact);
		std::vector<ExecutableWithCost> GetExecutablesUsingOnlyFacts(const std::vector<Fact> facts);
	};
}

#endif // universal_reasoner_metadata_h__
