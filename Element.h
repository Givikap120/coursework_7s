#pragma once

#include <string>

class Element
{
public:
	std::string Name;

protected:
	const double& CurrentTime;

public:
	Element(const double& modelTime) : CurrentTime(modelTime) {}
	virtual ~Element() {}
};

