#pragma once

#include "Arc.h"
#include "Place.h"
#include "Transition.h"

class ArcToPlace : public Arc
{
public:
	Transition* Input;
	Place* Output;

	ArcToPlace(const double& modelTime, Transition* input, Place* output, int connections = 1) : Arc(modelTime, connections), Input(input), Output(output) {}
};