#pragma once
#include "ArcToTransition.h"

class ArcToTransitionInformational : public ArcToTransition
{
public:
	ArcToTransitionInformational(const double& modelTime, Place* input, Transition* output, int connections = 1) : ArcToTransition(modelTime, input, output, connections) {}
	
	void TakeMarksFromPlace() override {}
};

