#pragma once

#include "Arc.h"
#include "Place.h"
#include "Transition.h"

class ArcToTransition : public Arc
{
public:
	Place* Input;
	Transition* Output;

	ArcToTransition(Model* model, Place* input, Transition* output, int connections = 1) : Arc(model, connections), Input(input), Output(output) {}

	bool IsTransitionAvailable() const { return Input->GetMarks() >= Connections; }
	virtual void TakeMarksFromPlace() { Input->TakeMarks(Connections); }
};