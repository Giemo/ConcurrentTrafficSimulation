#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _condition.wait(lock, [this]{ return  !_queue.empty();});

    T msg = std::move(_queue.front());
    _queue.pop_front();

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.emplace_back(msg);
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // Infinite loop is not necessary, because it can be checked if the TrafficLightPhase is red in the while(condition)
    while (_message_queue.receive() == TrafficLightPhase::red)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{     
    auto previous_update = std::chrono::system_clock::now();
    long time_between_loops;
    int cylce_duration;

    // Set cycle duration to random number between 4 and 6
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(4, 6);
    cylce_duration = dist(mt); 

    while(true)
    {
        time_between_loops = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - previous_update).count();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Only update TrafficLightPhase if enough time has passed
        if(time_between_loops >= cylce_duration)
        {
            _currentPhase = _currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;     

            // Send message
            auto msg = _currentPhase;
            _message_queue.send(std::move(_currentPhase));

            previous_update = std::chrono::system_clock::now();               
        }
    }
}
