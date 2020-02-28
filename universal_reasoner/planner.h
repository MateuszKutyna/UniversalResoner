#ifndef planner_h__
#define planner_h__

#include "metadata.h"

namespace ureasoner
{
	template<typename METADATA>
	class Planer
	{
	public:
		using Metadata = METADATA;
		Planer(shared_ptr<Metadata> metadata) : metadata(metadata) {}
	protected:
		const shared_ptr<Metadata> metadata;
	};
}
#endif // planner_h__
