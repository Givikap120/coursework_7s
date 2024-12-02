#pragma once

#include "RandomUtils.h"
#include "Model.h"

struct SimulationParameters
{
	double SimulationTime = 10000;

	int Bulldosers = 1;
	int Loaders1 = 1;
	int Loaders2 = 1;
	int DumpTracks = 4;
	int MinDirtQueue = 2;

	double BulldoserMean = 8;
	double BulldoserVariance = 32;
	double Loader1Work = 14;
	double Loader2Work = 12;
	double LoaderCooldown = 5;
	double DumpTrackTripToMean = 22;
	double DumpTrackTripBackMean = 18;
	double DumpTrackTripDeviation = 10;
	double DumpTrackUnloadingMean = 5;
	double DumpTrackUnloadingRange = 3;
};

struct SimulationResult
{
	double SimulatedTime = 0;
	double Productivity = 0;
	double AvgDirtQueue = 0, MaxDirtQueue = 0, AvgFreeLoaders = 0, AvgFreeDumpTracks = 0;
	double DirtBottleneck = 0, LoaderBottleneck = 0, DumpTrackBottleneck = 0;

	void print(int precision = -1) const;
	SimulationResult& operator+=(const SimulationResult& other);
	SimulationResult& operator/=(double value);
};

class DirtTransportationModel
{
	Model model;

	Place* pBulldoser = nullptr;
	Transition* tBulldoser = nullptr;

	Place* pDirtQueue = nullptr;

	Place* pLoader1 = nullptr;
	Transition* tLoading1 = nullptr;
	Place* pLoader1Cooldown = nullptr;
	Transition* tLoader1Cooldown = nullptr;

	Place* pLoader2 = nullptr;
	Transition* tLoading2 = nullptr;
	Place* pLoader2Cooldown = nullptr;
	Transition* tLoader2Cooldown = nullptr;

	Place* pFreeDumpTruck = nullptr;
	Place* pLoadedDumpTruck = nullptr;

	Transition* tTransportTo = nullptr;
	Place* pWaitingForUnload = nullptr;

	Transition* tUnload = nullptr;
	Place* pUnloaded = nullptr;

	Transition* tTransportBack = nullptr;

	std::vector<double> unsatisfiedInputTimes;

public:

	SimulationParameters p;
	bool IsReset = false;

	DirtTransportationModel() : p(SimulationParameters()) {}
	DirtTransportationModel(const SimulationParameters& p) : p(p) {}

	void Reset();

	SimulationResult Run(bool verbose);
	std::vector<SimulationResult> RunProgressive();

	SimulationResult RunAverage(int repeatsCount, bool verbose);
	std::vector<SimulationResult> RunBatch(int repeatsCount, bool verbose);
	std::vector<std::vector<SimulationResult>> RunProgressiveBatch(int repeatsCount, bool verbose);

	static void OutputToCSV(const std::vector<std::vector<SimulationResult>>& result, std::string filename, int resolution);

	static double FindProductivityMean(const std::vector<SimulationResult>& result);
	static double FindProductivityVariance(const std::vector<SimulationResult>& result);
	static double FindProductivitySTD(const std::vector<SimulationResult>& result);
};

