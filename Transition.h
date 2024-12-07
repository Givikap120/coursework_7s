#pragma once

#include "Element.h"

#include <vector>
#include <functional>

#include "SortedArray.h"

class ArcToPlace;
class ArcToTransition;

class Transition : public Element
{
	std::vector<ArcToTransition*> inputArcs;
	std::vector<ArcToPlace*> outputArcs;

	SortedArray<double> eventTimes;

	// Statistics
	int totalFinished = 0;

public:
	Transition(const double& modelTime, std::function<double()> delay = []() {return 0; }) : Element(modelTime), DelayFunction(delay) {}

	std::function<double()> DelayFunction;

	int GetActiveCount() const { return (int)eventTimes.size(); }
	
	double virtual GetEventTime() const { return eventTimes.empty() ? std::numeric_limits<double>::infinity() : eventTimes[0]; }

	void AddInputArc(ArcToTransition* arc) { inputArcs.push_back(arc); }
	void AddOutputArc(ArcToPlace* arc) { outputArcs.push_back(arc); }

	bool IsValid() const { return inputArcs.size() > 0 && outputArcs.size() > 0; }

	bool IsTransitionAvailable() const;

	bool virtual ActivateTransition();
	void virtual FinishTransition();

	// Statistics
	double GetTotalTransitions() const { return totalFinished; }
};

