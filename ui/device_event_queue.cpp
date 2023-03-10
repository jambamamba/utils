#include "device_event_queue.h"
#include "screen_manager.h"

#include <json_gui_loader.h>

typedef struct ClickMouseT{
  lv_point_t _click_mouse_point;
  lv_indev_state_t _click_mouse_state;
  bool _processed;
}ClickMouse;
static ClickMouse _state;

static void setIndevDataOverride(const lv_point_t *point, lv_event_code_t event_code)
{
  if(event_code == LV_EVENT_CLICKED){
    _state._click_mouse_state = LV_INDEV_STATE_PRESSED;
  }
  else if(event_code == LV_EVENT_RELEASED){
    _state._click_mouse_state = LV_INDEV_STATE_RELEASED;
  }
  _state._click_mouse_point.x = point->x;
  _state._click_mouse_point.y = point->y;
  _state._processed = false;
}

extern "C" void defaultMouseReadHandler(struct _lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
extern "C" void sdl_mouse_read(struct _lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
extern "C" void mouseReadOverride(struct _lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
  defaultMouseReadHandler(indev_drv, data);
  if(data->state == LV_INDEV_STATE_PRESSED){
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@ pressed at (%i,%i) %i\n", data->point.x, data->point.y, data->state);
  }
  if(!_state._processed) {
    data->point.x = _state._click_mouse_point.x;
    data->point.y = _state._click_mouse_point.y;
    data->state = _state._click_mouse_state;
    _state._processed = true;
  }
}

void DeviceEventQueue::pushEventTouchScreenAtPoint(int32_t x, int32_t y){
    auto event = std::make_unique<DeviceEvent>();
    event->_type = DeviceEvent::DeviceEvent_TouchScreenAtPoint;
    event->_u._point.x = x;
    event->_u._point.y = y;
    event->_event_code = LV_EVENT_CLICKED;
    _events.emplace_front(std::move(event));
}
void DeviceEventQueue::pushEventTouchWidgetByText(lv_obj_t *obj){
    auto event = std::make_unique<DeviceEvent>();
    event->_type = DeviceEvent::DeviceEvent_TouchWidgetByText;
    event->_u._obj = obj;
    _events.emplace_front(std::move(event));
}
void DeviceEventQueue::pushEventTouchWidgetById(lv_obj_t *obj){
    auto event = std::make_unique<DeviceEvent>();
    event->_type = DeviceEvent::DeviceEvent_TouchWidgetById;
    event->_u._obj = obj;
    _events.emplace_front(std::move(event));
}
void DeviceEventQueue::handleDeviceEvent(){
    if(_events.size() == 0) {
        return;
    }
    auto &event = _events.back();
    if(!event->_handling_event){
        startHandlingDeviceEvent(event);
    }
    else {
        continueHandlingDeviceEvent(event);            
    }
}
void DeviceEventQueue::startHandlingDeviceEvent(std::unique_ptr<DeviceEvent> &event){
    event->_handling_event = true;
    event->_start_time = std::chrono::steady_clock::now();
    switch(event->_type){
        case DeviceEvent::DeviceEvent_TouchWidgetById:
        case DeviceEvent::DeviceEvent_TouchWidgetByText:{
            auto rect = ScreenManager::getScreenManager()->getObjectRect(event->_u._obj);
            if(!rect){
                return;
            }
            event->_canvas = drawCirclOnWidget(rect.value());
            event->_event_code = LV_EVENT_CLICKED;
            return;
        }
        case DeviceEvent::DeviceEvent_TouchScreenAtPoint:{
            lv_area_t area;
            area.x1 = area.x2 = event->_u._point.x;
            area.y1 = area.y2 =event->_u._point.y;
            event->_canvas = drawCirclOnWidget(area);
            event->_event_code = LV_EVENT_CLICKED;
            return;
        }                
        case DeviceEvent::DeviceEvent_None:{
            return;
        }
    }
}
void DeviceEventQueue::continueHandlingDeviceEvent(std::unique_ptr<DeviceEvent> &event){
    auto now = std::chrono::steady_clock::now();
    auto diff = now - event->_start_time;
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
    if(elapsed.count() > 100){
        if(event->_type == DeviceEvent::DeviceEvent_TouchScreenAtPoint){
            setIndevDataOverride(&event->_u._point, event->_event_code);
        }
        else {
            sendWidgetTouchEvent(event->_u._obj, event->_event_code);
        }
        if(event->_event_code == LV_EVENT_RELEASED){
            lv_obj_del(event->_canvas);
            _events.pop_back();
            return;
        }
        event->_start_time = std::chrono::steady_clock::now();
        event->_event_code = LV_EVENT_RELEASED;
    }
}
