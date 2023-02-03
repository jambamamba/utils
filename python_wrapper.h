#pragma once

#include <Python.h>//must be first

#include <functional>
#include <string>
#include <vector>

struct ScreenContext;
class PythonWrapper
{
public:
    explicit PythonWrapper();
    ~PythonWrapper();
    bool pythonInit(int argc, char **argv);
    long pythonCallMain(std::function<void()> event_loop);
    void onLoadScreenCallback(const char* screen_id);
    void registerTouchWidgetProcs(
        std::function <bool(const char* widget_id)> load_screen_by_id_proc,
        std::function <bool(const char* widget_id)> touch_widget_by_id_proc,
        std::function <bool(const char *obj_text, int nth_obj)> touch_widget_by_text_proc,
        std::function <bool(int32_t x, int32_t y)> touch_screen_at_point_proc,
        std::function <std::string(int32_t x, int32_t y)> get_widget_text_on_screen_at_proc);
    bool eventLoop();

    PyObject *_on_load_screen_callback = nullptr;
    std::function <bool(const char* widget_id)> _load_screen_by_id_proc;
    std::function <bool(const char* widget_id)> _touch_widget_by_id_proc;
    std::function <bool(const char *obj_text, int nth_obj)> _touch_widget_by_text_proc;
    std::function <bool(int32_t x, int32_t y)> _touch_screen_at_point_proc;
    std::function <std::string(int32_t x, int32_t y)> _get_widget_text_on_screen_at_proc;
    std::function <void()> _app_event_loop = nullptr;
    PyObject *_py_event_loop_callback = nullptr;
    std::function<void()> _py_event_loop = nullptr;
protected:
    bool _initialized = false;
    PyConfig _config;
    PyObject *_py_module = nullptr;
    std::string _py_script_to_run;
    static PyObject *pythonCallFunction(PyObject *pModule, const char* func, const std::vector<std::string> &args);
};