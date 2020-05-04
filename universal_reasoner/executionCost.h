#ifndef universal_reasoner_executionCost_h__
#define universal_reasoner_executionCost_h__

namespace ureasoner
{
	class ExecutionCost
	{
	public:
		typedef double CostType;

		ExecutionCost(CostType cost) noexcept :cost(cost) {};
		CostType EstimatedCost()  const noexcept{
			return cost;
		};
	protected:
		const CostType cost;
	};
}

#endif // universal_reasoner_executionCost_h__

