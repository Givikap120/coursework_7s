#pragma once
#include "ArcToTransition.h"

class ArcToTransitionInformational : public ArcToTransition
{
public:
	ArcToTransitionInformational(Model* model, Place* input, Transition* output, int connections = 1) : ArcToTransition(model, input, output, connections) {}
	
	int CountAvailableTransitions() const override { return Input->GetMarks() >= Connections ? std::numeric_limits<int>::max() : 0; }
	void GetMarksFromPlace(int activated) override {}
};

