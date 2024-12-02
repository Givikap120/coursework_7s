#pragma once

#include "Element.h"

class Arc : public Element
{
public:
	int Connections;

	Arc(Model* model, int connections) : Element(model), Connections(connections) {}
};

