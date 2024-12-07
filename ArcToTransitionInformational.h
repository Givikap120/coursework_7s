#pragma once
#include "ArcToTransition.h"

class ArcToTransitionInformational : public ArcToTransition
{
public:
	ArcToTransitionInformational(Model* model, Place* input, Transition* output, int connections = 1) : ArcToTransition(model, input, output, connections) {}
	
	void TakeMarksFromPlace() override {}
};

