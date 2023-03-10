#pragma once

#include <cstdint>
#include <functional>

//https://www.fluentcpp.com/2018/12/28/timer-cpp/
// typedef void (*TimerCallback)(uint32_t uiInput);

class SimpleTimer {
    bool _clear = false;

public:
    void setTimeout(std::function<bool()> callback, int delay);
    void setInterval(std::function<bool()> callback, int interval);
    void stop();
};
