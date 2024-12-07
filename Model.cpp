#include "Model.h"

#include <stdexcept>
#include <algorithm>
#include <unordered_set>

#include "RandomUtils.h"

#include "Place.h"
#include "Transition.h"

#include "ArcToPlace.h"
#include "ArcToTransitionInformational.h"

void Model::Simulate(double simulationTime)
{
    if (checkIfAllTransitionsValid() == false)
        throw std::logic_error("All transitions must have inputs and outputs");

    bool wasActivated;
    std::vector<Transition*> availableTransitions;
    availableTransitions.reserve(transitions.size());

    while (CurrentTime <= simulationTime) {

        finishTransitions();

        addAllAvailableTransitionsTo(availableTransitions);

        wasActivated = false;
        if (!availableTransitions.empty())
            wasActivated = RandomUtils::SelectRandomFrom(availableTransitions)->ActivateTransition();

        if (!wasActivated)
        {
            double nextTime = getNextEventTime();
            double deltaT = nextTime - CurrentTime;

            for (auto& module : StatisticCollectors) module(deltaT);
            CurrentTime = nextTime;
        } 

        availableTransitions.clear();
    }
}

bool Model::checkIfAllTransitionsValid() const
{
    for (auto transition : transitions)
    {
        if (!transition->IsValid()) return false;
    }

    return true;
}

void Model::finishTransitions()
{
    for (auto transition : transitions)
    {
        if (transition->GetEventTime() <= CurrentTime)
            transition->FinishTransition();
    }
}

void Model::addAllAvailableTransitionsTo(std::vector<Transition*>& targetTransitions)
{
    for (auto transition : transitions)
    {
        if (transition->IsTransitionAvailable())
            targetTransitions.push_back(transition);
    }
}

double Model::getNextEventTime() const
{
    double nextTime = std::numeric_limits<double>::infinity();
    for (auto transition : transitions)
    {
        nextTime = std::min(nextTime, transition->GetEventTime());
    }
    return nextTime;
}

void Model::Connect(Place* input, Transition* output, int connections)
{
    auto arc = std::make_unique<ArcToTransition>(this, input, output, connections);

    input->AddOutputArc(arc.get());
    output->AddInputArc(arc.get());

    storage.push_back(std::move(arc));
}

void Model::Connect(Transition* input, Place* output, int connections)
{
    auto arc = std::make_unique<ArcToPlace>(this, input, output, connections);

    input->AddOutputArc(arc.get());
    output->AddInputArc(arc.get());

    storage.push_back(std::move(arc));
}

void Model::ConnectInformational(Place* input, Transition* output, int connections)
{
    auto arc = std::make_unique<ArcToTransitionInformational>(this, input, output, connections);

    input->AddOutputArc(arc.get());
    output->AddInputArc(arc.get());

    storage.push_back(std::move(arc));
}

void Model::Connect(Place* input, Transition* transition, Place* output, int connectionsIn, int connectionsOut)
{
    Connect(input, transition, connectionsIn);
    Connect(transition, output, connectionsOut);
}

void Model::AddElement(std::unique_ptr<Element> element)
{
    if (auto place = dynamic_cast<Place*>(element.get())) {
        places.push_back(place);
    }
    else if (auto transition = dynamic_cast<Transition*>(element.get())) {
        transitions.push_back(transition);
    }
    else {
        throw std::invalid_argument("Element must be Place or Transition");
    }

    storage.push_back(std::move(element));
}