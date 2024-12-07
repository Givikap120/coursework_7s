#pragma once

#include "Element.h"

class Arc : public Element
{
public:
	int Connections;

	Arc(const double& modelTime, int connections) : Element(modelTime), Connections(connections) {}
};

