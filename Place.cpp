#include "Place.h"

void Place::updateTimeStats()
{
	double deltaT = CurrentTime - timeSinceTimeStatsUpdate;
	timeSinceTimeStatsUpdate = CurrentTime;

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
	return weightedMarksSum / CurrentTime;
}

int Place::GetMaxMarksCount()
{
	updateTimeStats();
	return maxMarks;
}
