#include "simple_timer.h"

#include <thread>

void 
SimpleTimer::setInterval(std::function<bool()> callback, int interval) {
    _clear = false;
    std::thread t([=, this]() {
        while(true) {
            if(_clear) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(_clear) return;
            callback();
        }
    });
    t.detach();
}

void 
SimpleTimer::setTimeout(std::function<bool()> callback, int delay) {
    _clear = false;
    std::thread t([=, this]() {
        if(_clear) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if(_clear) return;
        callback();
    });
    t.detach();
}

void 
SimpleTimer::stop() {
    _clear = true;
}

