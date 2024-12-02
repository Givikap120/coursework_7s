#pragma once

#include <string>

class Model;

class Element
{
public:
	std::string Name;

protected:
	Model* model;

public:
	Element(Model* model) : model(model) {}
	virtual ~Element() {}
};

