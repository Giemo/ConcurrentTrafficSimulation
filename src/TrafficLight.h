#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <future>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum class TrafficLightPhase
{
    red,
    green
};

template <class T>
class MessageQueue
{
public:
    T receive();
	void send(T &&msg);

private:
    std::deque<TrafficLightPhase> _queue;
    std::condition_variable _condition;
    std::mutex _mutex;

};

class TrafficLight : public TrafficObject
{
public:
    TrafficLight();

    TrafficLightPhase getCurrentPhase();
    void setCurrentPhase(const TrafficLightPhase& phase);

    void waitForGreen();
    void simulate();

private:
    void cycleThroughPhases();

    std::condition_variable _condition;
    std::mutex _mutex;
    MessageQueue<TrafficLightPhase> _message_queue;
    TrafficLightPhase _currentPhase;
};

#endif