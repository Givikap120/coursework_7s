#include "RandomUtils.h"

#include <cmath>
#include <algorithm>
#include <numeric>

#include "Transition.h"

static std::default_random_engine generator(0);

std::default_random_engine& RandomUtils::getGenerator()
{
    return generator;
}

void RandomUtils::ResetRandom(bool isRandomSeed)
{
    if (isRandomSeed)
    {
        std::random_device d{};
        generator.seed(d());
    }
    else
        generator.seed(0);
}

std::function<double()> RandomUtils::Constant(double constant)
{
    return [constant]() { return constant; };
}

std::function<double()> RandomUtils::Uniform(double mean, double delta)
{
    std::uniform_real_distribution<double> distribution(mean - delta, mean + delta);
    return [distribution]() mutable { return std::max(0.0, distribution(generator)); };
}

std::function<double()> RandomUtils::Normal(double mean, double stddev)
{
    std::normal_distribution<double> distribution(mean, stddev);
    return [distribution]() mutable { return std::max(0.0, distribution(generator)); };
}

std::function<double()> RandomUtils::Exponential(double mean)
{
    std::exponential_distribution<double> distribution(1.0 / mean);
    return [distribution]() mutable { return distribution(generator); };
}

std::function<double()> RandomUtils::Erlang(double mean, double variance) {
    // mean = k / lambda
    // variance = k / lambda^2
    // lambda = mean / variance
    // k = mean * lambda

    double lambda = mean / variance;
    double k = mean * lambda;
    
    std::gamma_distribution<double> distribution(k, 1.0 / lambda);
    return [distribution]() mutable { return distribution(generator); };
}