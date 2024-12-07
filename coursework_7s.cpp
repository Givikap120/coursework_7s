#include <iostream>
#include <format>
#include <array>

#include "Testing.h"

const int repeatCount = 2000;
static double round(double value, int digits);

static void basicRunOnce()
{
	DirtTransportationModel model;
	model.Run(false).print(3);
}

static void basicRunBatch()
{
	DirtTransportationModel model;
	model.RunAverage(repeatCount, false).print(3);
}

static void verification()
{
	// Set Parameters
	std::vector<SimulationParameters> params(30);
	params[1].SimulationTime = 50000;
	params[2].Bulldosers = 4;
	params[3].Loaders1 = 0;
	params[4].Loaders2 = 0;
	params[5].Loaders1 = 10;
	params[6].DumpTracks = 20;

	params[7].SimulationTime = 100;
	params[8].SimulationTime = 100;
	params[9].SimulationTime = 100;
	params[8].MinDirtQueue = 1;
	params[9].MinDirtQueue = 5;

	params[10].BulldoserMean = 2;
	params[11].BulldoserMean = 32;
	params[12].Loader1Work = 5;
	params[13].Loader1Work = 30;
	params[14].Loader2Work = 5;
	params[15].Loader2Work = 30;
	params[16].LoaderCooldown = 0;
	params[17].LoaderCooldown = 20;
	params[18].DumpTrackTripToMean = 5;
	params[19].DumpTrackTripToMean = 50;
	params[20].DumpTrackTripBackMean = 5;
	params[21].DumpTrackTripBackMean = 50;
	params[22].DumpTrackUnloadingMean = 0;
	params[23].DumpTrackUnloadingMean = 20;

	params[24].BulldoserVariance = 1;
	params[25].BulldoserVariance = 128;
	params[26].DumpTrackTripDeviation = 1;
	params[27].DumpTrackTripDeviation = 18;
	params[28].DumpTrackUnloadingRange = 0;
	params[29].DumpTrackUnloadingRange = 5;

	DirtTransportationModel model;

	for (size_t i = 0; i < params.size(); i++)
	{
		RandomUtils::ResetRandom(false);

		model.p = params[i];
		model.Reset();
		auto r = model.Run(false);

		std::cout << i << '\t' << round(r.Productivity, 3) << '\t' << round(r.AvgDirtQueue, 0);
		std::cout << '\t' << round(r.MaxDirtQueue, 0) << '\t' << round(r.AvgFreeLoaders, 3) << '\t' << round(r.AvgFreeDumpTracks, 3);
		std::cout << '\t' << round(r.DirtBottleneck, 3) << '\t' << round(r.LoaderBottleneck, 3) << '\t' << round(r.DumpTrackBottleneck, 3) << '\n';
	}
}

static void findOptimalSimulationTime()
{
	DirtTransportationModel model;
	auto graphs = model.RunProgressiveBatch(5, true);
	DirtTransportationModel::OutputToCSV(graphs, "graphs.csv", 1000);
}

static void findSTD()
{
	DirtTransportationModel model;
	auto result = model.RunBatch(10000, true);
	std::cout << "Productivity Standard Deviation = " << DirtTransportationModel::FindProductivitySTD(result) << '\n';
}

static void factorTest()
{
	double numberOfDumpTracksFactor; // Factor1
	double loaderCooldownTimeFactor; // Factor2
	double useType2LoaderFactor; // Factor3

	DirtTransportationModel model;

	double max = 0, sum = 0;

	for (useType2LoaderFactor = 1; useType2LoaderFactor >= -1; useType2LoaderFactor -= 2)
		for (loaderCooldownTimeFactor = 1; loaderCooldownTimeFactor >= -1; loaderCooldownTimeFactor -= 2)
			for (numberOfDumpTracksFactor = 1; numberOfDumpTracksFactor >= -1; numberOfDumpTracksFactor -= 2)
			{

				model.p.DumpTracks = int(5 + 1 * numberOfDumpTracksFactor);
				model.p.LoaderCooldown = 3 + 2 * loaderCooldownTimeFactor;

				if (useType2LoaderFactor == 1)
				{
					model.p.Loaders1 = 0;
					model.p.Loaders2 = 2;
				}
				else
				{
					model.p.Loaders1 = 1;
					model.p.Loaders2 = 1;
				}

				auto result = model.RunBatch(repeatCount, false);
				double mean = DirtTransportationModel::FindProductivityMean(result);
				double variance = DirtTransportationModel::FindProductivityVariance(result);

				// adjust variance
				variance *= (double)result.size() / (result.size() - 1);

				max = std::max(max, variance);
				sum += variance;

				std::cout << numberOfDumpTracksFactor << " " << loaderCooldownTimeFactor << " " << useType2LoaderFactor << ": ";
				std::cout << round(mean, 4);
				std::cout <<", D = " << round(variance * 1000, 4) << " * 10^(-3)";
				std::cout << "\n";

				RandomUtils::ResetRandom(false);
			}

	std::cout << "G = " << max / sum;
}

static void optimalRunOnce()
{
	DirtTransportationModel model;
	model.p.DumpTracks = 6;
	model.p.LoaderCooldown = 3;
	model.Run(false).print(3);
}

int main()
{
	//basicRunOnce();
	//basicRunBatch();
	//verification();
	//findOptimalSimulationTime();
	//findSTD();
	//factorTest();
	optimalRunOnce();
}

double round(double value, int digits)
{
	double scale = std::pow(10.0, digits);
	return std::round(value * scale) / scale;
}
