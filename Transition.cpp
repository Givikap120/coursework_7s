#include "Transition.h"
#include "Place.h"

#include "ArcToTransition.h"
#include "ArcToPlace.h"

#include "Model.h"

#include <limits>
#include <stdexcept>

#include <iostream>

int Transition::CountAvailableTransitions() const
{
	int min = std::numeric_limits<int>::max();

	for (auto arc : inputArcs)
	{
		int n = arc->CountAvailableTransitions();
		if (n == 0)
			return 0;

		min = std::min(n, min);
	}

	return min;
}

int Transition::ActivateAllTransitions()
{
	int activated = CountAvailableTransitions();
	if (activated == 0)
	{
		return 0;
	}

	for (auto arc : inputArcs)
	{
		arc->GetMarksFromPlace(activated);
	}

	std::vector<double> newEventTimes(activated);

	for (auto& eventTime : newEventTimes)
	{
		double newTime = model->CurrentTime + DelayFunction();
		eventTime = newTime;
	}

	eventTimes.batch_insert(std::move(newEventTimes));
	return activated;
}

void Transition::FinishTransition()
{
	auto finishedEvents = eventTimes.upper_bound(model->CurrentTime);

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
