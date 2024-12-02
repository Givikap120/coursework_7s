#include "Testing.h"

#include <iostream>
#include <fstream>

static double round(double value, int digits)
{
	if (digits < 0) return value;
	double scale = std::pow(10.0, digits);
	return std::round(value * scale) / scale;
}

void SimulationResult::print(int precision) const
{
	std::cout << "Simulation time - " << round(SimulatedTime, 0) << '\n';
	std::cout << "Productivity - " << round(Productivity, precision) << " dirt per minute\n\n";

	std::cout << "Average amount of dirt - " << round(AvgDirtQueue, 0) << '\n';
	std::cout << "Max amount of dirt - " << round(MaxDirtQueue, 0) << '\n';
	std::cout << "Average amount of free loaders - " << round(AvgFreeLoaders, precision) << '\n';
	std::cout << "Average free dump tracks - " << round(AvgFreeDumpTracks, precision) << "\n\n";

	std::cout << "Portion of time with no dirt - " << round(DirtBottleneck, precision) << '\n';
	std::cout << "Portion of time with no loaders - " << round(LoaderBottleneck, precision) << '\n';
	std::cout << "Portion of time with no dump tracks - " << round(DumpTrackBottleneck, precision) << "\n\n";
}

SimulationResult& SimulationResult::operator+=(const SimulationResult& other)
{
	SimulatedTime += other.SimulatedTime;
	Productivity += other.Productivity;
	AvgDirtQueue += other.AvgDirtQueue;
	MaxDirtQueue += other.MaxDirtQueue;
	AvgFreeLoaders += other.AvgFreeLoaders;
	AvgFreeDumpTracks += other.AvgFreeDumpTracks;
	DirtBottleneck += other.DirtBottleneck;
	LoaderBottleneck += other.LoaderBottleneck;
	DumpTrackBottleneck += other.DumpTrackBottleneck;
	return *this;
}

SimulationResult& SimulationResult::operator/=(double value)
{
	if (value == 0.0) throw std::invalid_argument("Division by zero");
	SimulatedTime /= value;
	Productivity /= value;
	AvgDirtQueue /= value;
	MaxDirtQueue /= value;
	AvgFreeLoaders /= value;
	AvgFreeDumpTracks /= value;
	DirtBottleneck /= value;
	LoaderBottleneck /= value;
	DumpTrackBottleneck /= value;
	return *this;
}

void DirtTransportationModel::Reset()
{
	if (IsReset) return;

	model = Model();

	pBulldoser = model.CreateNamedElement<Place>("pBulldoser", p.Bulldosers);
	tBulldoser = model.CreateNamedElement<Transition>("tBulldoser", RandomUtils::Erlang(p.BulldoserMean, p.BulldoserVariance));
	model.Connect(pBulldoser, tBulldoser, pBulldoser);

	pDirtQueue = model.CreateNamedElement<Place>("pDirtQueue");
	model.Connect(tBulldoser, pDirtQueue);

	pLoader1 = model.CreateNamedElement<Place>("pLoader1", p.Loaders1);
	tLoading1 = model.CreateNamedElement<Transition>("tLoading1", RandomUtils::Exponential(p.Loader1Work));
	pLoader1Cooldown = model.CreateNamedElement<Place>("pLoader1Cooldown");
	tLoader1Cooldown = model.CreateNamedElement<Transition>("tLoader1Cooldown", RandomUtils::Constant(p.LoaderCooldown));
	model.Connect(pLoader1, tLoading1, pLoader1Cooldown);
	model.Connect(pLoader1Cooldown, tLoader1Cooldown, pLoader1);

	pLoader2 = model.CreateNamedElement<Place>("pLoader2", p.Loaders2);
	tLoading2 = model.CreateNamedElement<Transition>("tLoading2", RandomUtils::Exponential(p.Loader2Work));
	pLoader2Cooldown = model.CreateNamedElement<Place>("pLoader2Cooldown");
	tLoader2Cooldown = model.CreateNamedElement<Transition>("tLoader2Cooldown", RandomUtils::Constant(p.LoaderCooldown));
	model.Connect(pLoader2, tLoading2, pLoader2Cooldown);
	model.Connect(pLoader2Cooldown, tLoader2Cooldown, pLoader2);

	pFreeDumpTruck = model.CreateNamedElement<Place>("pFreeDumpTruck", p.DumpTracks);
	pLoadedDumpTruck = model.CreateNamedElement<Place>("pLoadedDumpTruck");

	model.Connect(pFreeDumpTruck, tLoading1);
	model.Connect(pFreeDumpTruck, tLoading2);

	model.Connect(pDirtQueue, tLoading1, pLoadedDumpTruck);
	model.Connect(pDirtQueue, tLoading2, pLoadedDumpTruck);

	model.ConnectInformational(pDirtQueue, tLoading1, p.MinDirtQueue);
	model.ConnectInformational(pDirtQueue, tLoading2, p.MinDirtQueue);

	tTransportTo = model.CreateNamedElement<Transition>("tTransportTo", RandomUtils::Normal(p.DumpTrackTripToMean, p.DumpTrackTripDeviation));
	pWaitingForUnload = model.CreateNamedElement<Place>("pWaitingForUnload");
	model.Connect(pLoadedDumpTruck, tTransportTo, pWaitingForUnload);

	tUnload = model.CreateNamedElement<Transition>("tUnload", RandomUtils::Uniform(p.DumpTrackUnloadingMean, p.DumpTrackUnloadingRange));
	pUnloaded = model.CreateNamedElement<Place>("pUnloaded");
	model.Connect(pWaitingForUnload, tUnload, pUnloaded);

	tTransportBack = model.CreateNamedElement<Transition>("tTransportBack", RandomUtils::Normal(p.DumpTrackTripBackMean, p.DumpTrackTripDeviation));
	model.Connect(pUnloaded, tTransportBack, pFreeDumpTruck);

	unsatisfiedInputTimes = { 0, 0, 0 };
	model.StatisticCollectors.push_back([=, &unsatisfiedInputTimes = unsatisfiedInputTimes](double deltaT)
		{
			if (pDirtQueue->GetMarks() < p.MinDirtQueue) unsatisfiedInputTimes[0] += deltaT;
			if (pLoader1->GetMarks() < 1 && pLoader2->GetMarks() < 1) unsatisfiedInputTimes[1] += deltaT;
			if (pFreeDumpTruck->GetMarks() < 1) unsatisfiedInputTimes[2] += deltaT;
		});

	IsReset = true;
}

SimulationResult DirtTransportationModel::Run(bool verbose)
{
	Reset();
	model.Simulate(p.SimulationTime);
	IsReset = false;

	SimulationResult result{};

	result.AvgDirtQueue = pDirtQueue->GetAverageMarksCount();
	result.MaxDirtQueue = pDirtQueue->GetMaxMarksCount();
	result.AvgFreeLoaders = pLoader1->GetAverageMarksCount() + pLoader2->GetAverageMarksCount();
	result.AvgFreeDumpTracks = pFreeDumpTruck->GetAverageMarksCount();

	result.DirtBottleneck = unsatisfiedInputTimes[0] / model.CurrentTime;
	result.LoaderBottleneck = unsatisfiedInputTimes[1] / model.CurrentTime;
	result.DumpTrackBottleneck = unsatisfiedInputTimes[2] / model.CurrentTime;

	result.Productivity = tUnload->GetTotalTransitions() / model.CurrentTime;
	result.SimulatedTime = model.CurrentTime;

	if (verbose) result.print();

	return result;
}

std::vector<SimulationResult> DirtTransportationModel::RunProgressive()
{
	Reset();

	std::vector<SimulationResult> progressiveResult;
	double previousDirt = -1;

	model.StatisticCollectors.push_back([&](double deltaT)
		{
			SimulationResult result{};
			double currentTime = model.CurrentTime == 0 ? std::numeric_limits<double>::infinity() : model.CurrentTime;

			result.AvgDirtQueue = pDirtQueue->GetAverageMarksCount();
			result.MaxDirtQueue = pDirtQueue->GetMaxMarksCount();
			result.AvgFreeLoaders = pLoader1->GetAverageMarksCount() + pLoader2->GetAverageMarksCount();
			result.AvgFreeDumpTracks = pFreeDumpTruck->GetAverageMarksCount();

			result.DirtBottleneck = unsatisfiedInputTimes[0] / currentTime;
			result.LoaderBottleneck = unsatisfiedInputTimes[1] / currentTime;
			result.DumpTrackBottleneck = unsatisfiedInputTimes[2] / currentTime;

			double dirtProcessed = tUnload->GetTotalTransitions();

			if (previousDirt == dirtProcessed)
				return;

			result.Productivity = dirtProcessed / currentTime;
			result.SimulatedTime = model.CurrentTime;

			previousDirt = dirtProcessed;
			progressiveResult.push_back(result);
		});

	model.Simulate(p.SimulationTime);
	IsReset = false;
	return progressiveResult;
}

SimulationResult DirtTransportationModel::RunAverage(int repeatsCount, bool verbose)
{
	SimulationResult sum{};

	for (int i = 0; i < repeatsCount; i++)
	{
		sum += Run(false);

		if (verbose && i > 0 && i % (repeatsCount / 10) == 0)
			std::cout << (100 * i / repeatsCount) << "%\n";
	}

	if (verbose) std::cout << '\n';

	sum /= repeatsCount;
	return sum;
}

std::vector<SimulationResult> DirtTransportationModel::RunBatch(int repeatsCount, bool verbose)
{
	std::vector<SimulationResult> result;

	for (int i = 0; i < repeatsCount; i++)
	{
		result.push_back(Run(false));

		if (verbose && i > 0 && i % (repeatsCount / 10) == 0)
			std::cout << (100 * i / repeatsCount) << "%\n";
	}

	if (verbose) std::cout << '\n';

	return result;
}

std::vector<std::vector<SimulationResult>> DirtTransportationModel::RunProgressiveBatch(int repeatsCount, bool verbose)
{
	std::vector<std::vector<SimulationResult>> result;

	for (int i = 0; i < repeatsCount; i++)
	{
		result.push_back(RunProgressive());

		if (verbose)
			std::cout << i + 1 << "/" << repeatsCount << "\n";
	}

	if (verbose) std::cout << '\n';

	return result;
}

static double interpolateProductivity(const std::vector<SimulationResult>& results, double time)
{
	if (time <= results.front().SimulatedTime) {
		return results.front().Productivity;
	}
	if (time >= results.back().SimulatedTime) {
		return results.back().Productivity;
	}

	for (size_t i = 0; i < results.size() - 1; ++i) {
		if (results[i].SimulatedTime <= time && results[i + 1].SimulatedTime >= time) {
			double t1 = results[i].SimulatedTime;
			double t2 = results[i + 1].SimulatedTime;
			double p1 = results[i].Productivity;
			double p2 = results[i + 1].Productivity;

			// Linear interpolation formula
			return p1 + (p2 - p1) * (time - t1) / (t2 - t1);
		}
	}

	return 0.0;
}

void DirtTransportationModel::OutputToCSV(const std::vector<std::vector<SimulationResult>>& results, std::string filename = "simulation_results.csv", int resolution = 1000)
{
	std::ofstream outFile(filename);
	if (!outFile.is_open()) {
		throw std::ios_base::failure("Failed to open the output file.");
	}

	// Find the maximum SimulationTime across all runs
	double maxSimulationTime = 0.0;
	for (const auto& run : results) {
		if (!run.empty()) {
			maxSimulationTime = std::max(maxSimulationTime, run.back().SimulatedTime);
		}
	}

	// Generate time points
	std::vector<double> timePoints;
	for (size_t i = 0; i <= resolution; ++i) {
		timePoints.push_back(maxSimulationTime * i / resolution);
	}

	// Write the header
	outFile << "Run";
	for (size_t i = 0; i < results.size(); i++)
	{
		outFile << ',' << i + 1;
	}
	outFile << "\n";

	// Interpolate and write rows
	for (auto timePoint : timePoints) {
		outFile << timePoint;

		for (size_t runIndex = 0; runIndex < results.size(); ++runIndex) {
			outFile << ',' << interpolateProductivity(results[runIndex], timePoint);
		}

		outFile << '\n';
	}

	// Close the file
	outFile.close();
}

double DirtTransportationModel::FindProductivityMean(const std::vector<SimulationResult>& results)
{
	if (results.empty()) {
		throw std::invalid_argument("Vector is empty.");
	}

	double mean = 0.0;
	for (const auto& result : results) {
		mean += result.Productivity;
	}
	mean /= results.size();

	return mean;
}

double DirtTransportationModel::FindProductivityVariance(const std::vector<SimulationResult>& results)
{
	double mean = FindProductivityMean(results);

	double variance = 0.0;
	for (const auto& result : results) {
		const double& value = result.Productivity;
		variance += (value - mean) * (value - mean);
	}
	variance /= results.size();

	return variance;
}

double DirtTransportationModel::FindProductivitySTD(const std::vector<SimulationResult>& results)
{
	return std::sqrt(FindProductivityVariance(results));
}
