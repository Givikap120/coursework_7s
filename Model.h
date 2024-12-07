#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "Arc.h"
#include "Place.h"
#include "Transition.h"

class Model
{
	std::vector<std::unique_ptr<Element>> storage;

	std::vector<Place*> places;
	std::vector<Transition*> transitions;

	bool checkIfAllTransitionsValid() const;

	void finishTransitions();
	void addAllAvailableTransitionsTo(std::vector<Transition*>& transitions);
	double getNextEventTime() const;

public:
	std::vector<std::function<void(double)>> StatisticCollectors;
	double CurrentTime = 0;

	void AddElement(std::unique_ptr<Element> element);

	void Simulate(double simulationTime);

	template <typename T, typename... Args>
	T* CreateElement(Args&&... args) {
		static_assert(std::is_base_of<Element, T>::value, "T must be derived from Element");

		std::unique_ptr<T> elementObject = std::make_unique<T>(CurrentTime, std::forward<Args>(args)...);
		T* element = elementObject.get();
		AddElement(std::move(elementObject));

		return element;
	}

	template <typename T, typename... Args>
	T* CreateNamedElement(std::string name, Args&&... args) {
		static_assert(std::is_base_of<Element, T>::value, "T must be derived from Element");

		std::unique_ptr<T> elementObject = std::make_unique<T>(CurrentTime, std::forward<Args>(args)...);
		T* element = elementObject.get();
		element->Name = name;
		AddElement(std::move(elementObject));

		return element;
	}

	void Connect(Place* input, Transition* output, int connections = 1);
	void ConnectInformational(Place* input, Transition* output, int connections = 1);

	void Connect(Transition* input, Place* output, int connections = 1);

	void Connect(Place* input, Transition* transition, Place* output, int connectionsIn = 1, int connectionsOut = 1);
};

