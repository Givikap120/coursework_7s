#pragma once

#include <random>
#include <functional>
#include <unordered_set>

class RandomUtils
{
	static std::default_random_engine& getGenerator();
public:
	// Generators
	static std::function<double()> Constant(double constant);
	static std::function<double()> Uniform(double mean, double delta);
	static std::function<double()> Normal(double mean, double stddev);
	static std::function<double()> Exponential(double mean);
	static std::function<double()> Erlang(double mean, double variance);

	// Misc
	static void ResetRandom(bool isRandomSeed);

	template <class T> static T& SelectRandomFrom(std::vector<T>& arr)
	{
		return arr[getGenerator()() % arr.size()];
	}
};

