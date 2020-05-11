#include <iostream>
#include <random>
#include <deque>
#include <future>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function.
    std::unique_lock<std::mutex> _unique_lock(_mutex);
    _cond.wait(_unique_lock,[this]{return !_queue.empty();});

    T message = std::move(_queue.back());
    _queue.pop_back();
    return message;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> _unique_lock(_mutex);
    _queue.push_back(std::move(msg));
    _cond.notify_one();    
}


/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
    _current_phase = TrafficLightPhase::red;
    _message_que = std::make_shared<MessageQueue <TrafficLightPhase> >();
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true){
        if(_message_que->receive() == TrafficLightPhase::green)
            return;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));


    }
}

TrafficLightPhase TrafficLight::getCurrentPhase() const
{
    return _current_phase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    auto _last_update = std::chrono::system_clock::now();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(4,6);
    int cycle_duration = distribution(gen);
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        int time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - _last_update).count();
        if(time_elapsed >= cycle_duration){
            _current_phase = (_current_phase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red ;

            _message_que->send(std::move(_current_phase));

            _last_update = std::chrono::system_clock::now();
            cycle_duration = distribution(gen);
        }
    }

}

