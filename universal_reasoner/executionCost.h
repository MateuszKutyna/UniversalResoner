#ifndef universal_reasoner_executionCost_h__
#define universal_reasoner_executionCost_h__

namespace ureasoner
{
	class ExecutionCost
	{
	public:
		typedef double CostType;

		ExecutionCost(CostType cost) :cost(cost) {};
		CostType EstimatedCost() const{
			return cost;
		};
	protected:
		const CostType cost;
	};
}

#endif // universal_reasoner_executionCost_h__

