#include "Place.h"
#include "Model.h"

void Place::updateTimeStats()
{
	double deltaT = model->CurrentTime - timeSinceTimeStatsUpdate;
	timeSinceTimeStatsUpdate = model->CurrentTime;

	weightedMarksSum += marks * deltaT;
	maxMarks = std::max(maxMarks, marks);
}

void Place::AddMarks(int N)
{
	updateTimeStats();
	marks += N;
}

void Place::TakeMarks(int N)
{
	updateTimeStats();
	marks -= N;
}

double Place::GetAverageMarksCount()
{
	updateTimeStats();
	return weightedMarksSum / model->CurrentTime;
}

int Place::GetMaxMarksCount()
{
	updateTimeStats();
	return maxMarks;
}
