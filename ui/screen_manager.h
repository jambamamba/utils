#pragma once

#include <functional>
#include <json_gui_loader.h>
#include <lvgl/lvgl.h>
#include <list>
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>

using CloseScreenCallback = void(*)(const char *);

struct ScreenContext{
    SCREEN_TYPE_E _screen_id;
    lv_obj_t * _screen_handle;
    std::map<_lv_obj_t *, std::vector<_lv_obj_t *>> _children;
    ScreenContext(SCREEN_TYPE_E screen_id, lv_obj_t *handle)
        : _screen_id(screen_id)
        , _screen_handle(handle)
    {}
};

class ScreenManager
{
public:
    ScreenManager(const ScreenManager&) = delete;
    ScreenManager& operator=(const ScreenManager&) = delete;
    ScreenManager(ScreenManager&&) = delete;
    ScreenManager& operator=(ScreenManager&&) = delete;
    ~ScreenManager() = default;

    void clearScreenStack();
    void loadScreenById(const char* widget_id);
    void loadScreenByScreenEnum(SCREEN_TYPE_E screen_id);
    void loadScreen(lv_obj_t *screen, const char *screen_id);
    void loadScreen(lv_obj_t *previous_screen, SCREEN_TYPE_E screen_id);
    void unLoadScreen();
    void registerLoadScreenCallback(std::function<void(const char* screen_id)> callback);
    void registerCloseScreenCallback(const char *screen_id, CloseScreenCallback callback);
    void registerCloseScreenCallback(const std::string &screen_id, CloseScreenCallback callback);
    SCREEN_TYPE_E getPreviousScreenId();
    SCREEN_TYPE_E getCurrentScreenId();
    void updateObjectTree();
    std::vector<lv_obj_t *> findObjectsByLabelText(const char* needle) const;
    std::vector<lv_obj_t *> findObjectsAtPosition(int32_t x, int32_t y) const;
    std::optional<lv_area_t> getObjectRect(const lv_obj_t *obj, int depth = 0) const;
    static ScreenManager* getScreenManager();
private:
    ScreenManager();
    void invokeCloseScreenCallback(SCREEN_TYPE_E screen_id);
    std::unordered_map<int, CloseScreenCallback> _on_close_screen_callbacks;
    std::list<ScreenContext> _screen_stack;
    ScreenContext _current_screen;
};
