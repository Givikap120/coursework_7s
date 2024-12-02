#pragma once

#include "Element.h"
#include <vector>

class ArcToPlace;
class ArcToTransition;

class Place : public Element
{
	int marks;

	std::vector<ArcToPlace*> inputArcs;
	std::vector<ArcToTransition*> outputArcs;

	// Statistics
	double timeSinceTimeStatsUpdate = 0;

	int maxMarks = -1;
	double weightedMarksSum = 0;

	void updateTimeStats();
public:
	Place(Model* model, int currentMarks = 0) : Element(model), marks(currentMarks) {}

	void AddInputArc(ArcToPlace* arc) { inputArcs.push_back(arc); }
	void AddOutputArc(ArcToTransition* arc) { outputArcs.push_back(arc); }

	const std::vector<ArcToPlace*> GetInputArcs() const { return inputArcs; }
	const std::vector<ArcToTransition*> GetOutputArcs() const { return outputArcs; }

	int GetMarks() const { return marks; }
	void AddMarks(int N);
	void TakeMarks(int N);

	// Statistics
	double GetAverageMarksCount();
	int GetMaxMarksCount();
};

