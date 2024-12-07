#include "Transition.h"
#include "Place.h"

#include "ArcToTransition.h"
#include "ArcToPlace.h"

#include <limits>
#include <stdexcept>

#include <iostream>

bool Transition::IsTransitionAvailable() const
{
	for (auto arc : inputArcs)
	{
		bool isAvailable = arc->IsTransitionAvailable();
		if (!isAvailable)
			return false;
	}

	return true;
}

bool Transition::ActivateTransition()
{
	bool activated = IsTransitionAvailable();
	if (!activated)
		return false;

	for (auto arc : inputArcs)
	{
		arc->TakeMarksFromPlace();
	}

	double eventTime = CurrentTime + DelayFunction();

	eventTimes.insert(std::move(eventTime));
	return activated;
}

void Transition::FinishTransition()
{
	auto finishedEvents = eventTimes.upper_bound(CurrentTime);

	int finishedAmount = (int)std::distance(eventTimes.begin(), finishedEvents);
	if (finishedAmount == 0) return;

	eventTimes.erase(eventTimes.begin(), finishedEvents);

	for (auto arc : outputArcs)
	{
		Place* place = arc->Output;
		place->AddMarks(arc->Connections * finishedAmount);
	}

	totalFinished += finishedAmount;
}
