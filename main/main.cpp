
#include <iostream>
#include <factsRepository.h>
#include <import.h>
#include <metadata.h>
#include <memory>
#include <planner.h>
#include <string>
#include<type_traits>
#include<concepts>
#include<string>

using namespace ureasoner;
using COST = double;

int main()
{

	auto f1 = std::make_shared<FactSettable<double>>();
	double d = 2.0;
	auto conclusion1 = std::make_shared<ConclusionSettingFact<double>>(f1, d);

	auto a1 = std::make_shared<FactConst<double>>(2.0);
	std::shared_ptr<Premise<double>> premis1 = std::make_shared<PremiseWithType<double>>(a1, 2.0);


	RuleImpl<double> rule1(premis1, conclusion1);
	//Not set so logic error

	//Basicly sets fact 
	rule1.CheckAndFire();
	//Because checkAndFire is setting value of the fact we can get it's value
	auto res = f1->GetValue();

	
	
}

