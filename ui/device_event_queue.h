#pragma once

#include <lvgl/lvgl.h>
#include <lvgl/src/draw/lv_draw_rect.h>
#include <lvgl/src/widgets/canvas/lv_canvas.h>
#include <chrono>
#include <deque>
#include <memory>

struct DeviceEvent {
    enum Type_E{
        DeviceEvent_None,
        DeviceEvent_TouchWidgetById,
        DeviceEvent_TouchWidgetByText,
        DeviceEvent_TouchScreenAtPoint
    };
    union DeviceEventU{
        DeviceEventU(){}
        DeviceEventU(const DeviceEventU&){}
        ~DeviceEventU(){}
        lv_point_t _point;
        lv_obj_t *_obj;
    }_u;
    lv_obj_t *_canvas = nullptr;
    bool _handling_event = false;
    lv_event_code_t _event_code = LV_EVENT_ALL;//0
    decltype(std::chrono::steady_clock::now()) _start_time;
    Type_E _type;

};
struct DeviceEventQueue {
    void pushEventTouchScreenAtPoint(int32_t x, int32_t y);
    void pushEventTouchWidgetByText(lv_obj_t *obj);
    void pushEventTouchWidgetById(lv_obj_t *obj);
    void handleDeviceEvent();
    void startHandlingDeviceEvent(std::unique_ptr<DeviceEvent> &event);
    void continueHandlingDeviceEvent(std::unique_ptr<DeviceEvent> &event);

    std::deque<std::unique_ptr<DeviceEvent>> _events;
};
