
#include "python_wrapper.h"

#include <cstddef>
#include <filesystem>
#include <debug_logger.h>
#include <mq_utils.h>
#include <sstream>
#include <vector>

#include "json_utils.h"

//grep -nr lv_draw_sw_rect /opt/usr_data/sdk/sysroots/aarch64-fslc-linux/usr/src/debug/nextgen-linux/0.1-r0/gui/libs/lvgl/src/*

LOG_CATEGORY(PY, "PY")

namespace {

static std::string trim(const char* str) {
    if(!str || !*str){
        return std::string();
    }
    std::string s(str);
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        std::not1(std::ptr_fun<int, int>(std::isspace))));
    s.erase(std::find_if(s.rbegin(), s.rend(),
        std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

static std::vector<std::wstring> getModulePathsFromJson(const char *file_name)
{
    std::vector<std::wstring> paths;
    char *gui_json_string = load_json_from_file(file_name);
    if (!gui_json_string) {
        return paths;
    }

    cJSON *json_obj = cJSON_Parse(gui_json_string);
    if (json_obj == NULL){
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL){
            LOG(WARNING, PY, "Error before: '%s'\n", error_ptr);
        }
        return paths;
    }
    cJSON *py_obj = cJSON_GetObjectItemCaseSensitive(json_obj, "python");
    cJSON *module_search_path = cJSON_GetObjectItemCaseSensitive(py_obj, "module_search_path");
    int len = cJSON_GetArraySize(module_search_path);
    for(int i = 0; i < len; ++i){
        std::string path = cJSON_GetArrayItem(module_search_path, i)->valuestring;
        std::wstring wide_string = std::wstring(path.begin(), path.end());
        paths.emplace_back(wide_string.c_str());
    }
    
    free(gui_json_string);
    return paths;
}

static std::vector<std::wstring> &appendModulePathOfScriptToRun(std::vector<std::wstring> &module_paths, int argc, char **argv)
{
    if(argc > 1) {
        // int update_hz = int_from_json_array(argv[1], "update_hz", 100, 0);
        const char *py_main = string_from_json_array(argv[1], "py_main");
        if(!py_main){
            LOG(WARNING, PY, "py_main was not found in args section of services.json, will skip loading python module\n");
            return module_paths;
        }
        std::string path(std::filesystem::path(py_main).parent_path());
        std::wstring wide_string = std::wstring(path.begin(), path.end());
        module_paths.emplace_back(wide_string.c_str());
        // _py_script_to_run = path.stem();
    }
    return module_paths;
}

static std::string getPythonScriptToRun(int argc, char **argv)
{
    if(argc > 1) {
        // int update_hz = int_from_json_array(argv[1], "update_hz", 100, 0);
        const char *py_main = string_from_json_array(argv[1], "py_main");
        if(!py_main){
            LOG(WARNING, PY, "py_main was not found in args section of services.json, will skip loading python module\n");
            return "";
        }
        return std::filesystem::path(py_main).stem();
    }
    return "";
}

static PythonWrapper *_this;

//Functions that py script can call
static PyObject* mydeviceRun(PyObject *self, PyObject *args);
static PyObject* mydeviceVersion(PyObject *self, PyObject *args);
//////////////
static PyObject* mydeviceLoadScreenById(PyObject *self, PyObject *args);
static PyObject* mydeviceOnLoadScreen(PyObject *self, PyObject *args);
static PyObject* mydeviceTouchWidgetById(PyObject *self, PyObject *args);
static PyObject* mydeviceTouchWidgetByText(PyObject *self, PyObject *args);
static PyObject* mydeviceTouchScreenAt(PyObject *self, PyObject *args);
static PyObject* mydeviceGetWidgetTextOnScreenAt(PyObject *self, PyObject *args);
//////////////
static PyObject* mydeviceWrite(PyObject *self, PyObject *args);//needed for routing sys.stderr and sys.stdout
static PyObject* mydevicePrint(PyObject *self, PyObject *args);//needed for routing sys.stderr and sys.stdout
static PyObject* mydeviceFlush(PyObject *self, PyObject *args);//needed for routing sys.stderr and sys.stdout

static PyMethodDef device_methods[] = {
   {"loadScreenById", mydeviceLoadScreenById, METH_VARARGS, "mydevice.loadScreenById(widget_id)"},
   {"onLoadScreen", mydeviceOnLoadScreen, METH_VARARGS, "mydevice.onLoadScreen(lambda)"},
   {"touchWidgetById", mydeviceTouchWidgetById, METH_VARARGS, "mydevice.touchWidgetById(widget_id)"},
   {"touchWidgetByText", mydeviceTouchWidgetByText, METH_VARARGS, "mydevice.touchWidgetByText(widget_text, nth_widget_with_same_text)"},
   {"touchScreenAt", mydeviceTouchScreenAt, METH_VARARGS, "mydevice.touchScreenAt(x, y)"},
   {"getWidgetTextOnScreenAt", mydeviceGetWidgetTextOnScreenAt, METH_VARARGS, "mydevice.getWidgetTextOnScreenAt(x, y)"},
   {"run", mydeviceRun, METH_VARARGS, "mydevice.run()"},
   {"version", mydeviceVersion, METH_VARARGS, "mydevice.version()"},
   ////////////////
   {"write", mydeviceWrite, METH_VARARGS, "mydevice.write(msg)"},
   {"print", mydevicePrint, METH_VARARGS, "mydevice.print(msg)"},
   {"flush", mydeviceFlush, METH_VARARGS, "mydevice.flush()"},
//    {"stop", mydevice_stop, METH_VARARGS, "mydevice.stop()"},
   {NULL, NULL, 0, NULL}
};

static PyModuleDef device_module = {
    PyModuleDef_HEAD_INIT, "mydevice", NULL, -1, device_methods,
    NULL, NULL, NULL, NULL
};

static PyObject* pyInitMyModule(void)
{
    return PyModule_Create(&device_module);
}
static std::string getStderrText()
{
    PyObject* stderr = PySys_GetObject("stderr"); // borrowed reference
    PyObject *value = nullptr, *encoded = nullptr;

    std::string result;
    char* temp_result = nullptr;
    Py_ssize_t size = 0;

    value =  PyObject_CallMethod(stderr,"getvalue", nullptr);
    if (!value) goto done;
    // ideally should get the preferred encoding
    encoded = PyUnicode_AsEncodedString(value, "utf-8", "strict");
    if (!encoded) goto done;
    if (PyBytes_AsStringAndSize(encoded, &temp_result, &size) == -1) goto done;
    size += 1;

    // copy so we own the memory
    result = (char*)malloc(sizeof(char)*size);
    for (int i = 0; i<size; ++i) {
        result.push_back(temp_result[i]);
    }

    done:
    Py_XDECREF(encoded);
    Py_XDECREF(value);

    return result;

}
static bool setupStderr() 
{
    PyObject *io = NULL, *stringio = NULL, *stringioinstance = NULL;

    bool success = false;

    io = PyImport_ImportModule("io");
    if (!io) goto done;
    stringio = PyObject_GetAttrString(io,"StringIO");
    if (!stringio) goto done;
    stringioinstance = PyObject_CallFunctionObjArgs(stringio,NULL);
    if (!stringioinstance) goto done;

    if (PySys_SetObject("stderr",stringioinstance)==-1) goto done;

    success = true;

    done:
    Py_XDECREF(stringioinstance);
    Py_XDECREF(stringio);
    Py_XDECREF(io);
    return success;
}

static PyObject* mydeviceWrite(PyObject */*self*/, PyObject *args)
{
    char *msg = 0;
    if(!PyArg_ParseTuple(args, "s", &msg)){
        return PyLong_FromLong(0);
    }

   std::string str = trim(msg);
   if(str.size() > 0){
      LOG(DEBUG, PY, "%s\n", str.c_str());
   }

   return PyLong_FromLong(1);
}

static PyObject* mydevicePrint(PyObject *self, PyObject *args)
{
   return mydeviceWrite(self, args);
}

static PyObject* mydeviceFlush(PyObject */*self*/, PyObject */*args*/)
{
   return PyLong_FromLong(1);
}

static PyObject* mydeviceVersion(PyObject */*self*/, PyObject *args)
{
   int major = 1;//(int)(This->GetVersion().toFloat());
   int minor = 2;//(int)((This->GetVersion().toFloat() - major) * 100.);
   PyObject *dict = PyDict_New();
   PyDict_SetItem(dict, PyUnicode_FromString("Major"), PyLong_FromDouble(major));
   PyDict_SetItem(dict, PyUnicode_FromString("Minor"), PyLong_FromDouble(minor));

   return dict;
}

static PyObject* mydeviceRun(PyObject */*self*/, PyObject *args)
{
   if(!PyArg_ParseTuple(args, "O", &_this->_py_event_loop_callback)){
        return PyBool_FromLong(false);
    }
    Py_INCREF(_this->_py_event_loop_callback);

    if(_this->_app_event_loop){
        _this->_app_event_loop();
    }
    return Py_BuildValue("i", 0);
}

static PyObject *argsToTuple(const std::vector<std::string> &args)
{
    PyObject *tuple = PyTuple_New(args.size());
    for (int i = 0; i < args.size(); ++i){
        std::vector<std::string> tokens;
        std::istringstream f(args[i]);
        std::string s;
        while (getline(f, s, ':')) {
            tokens.push_back(s);
        }
        PyObject *value;
        if(tokens.at(0)[0] == 's'){
            value = PyUnicode_FromString(tokens.at(1).c_str());
        }
        else if(tokens.at(0)[0] == 'i'){
            value = PyLong_FromLong(std::stoi(tokens.at(1)));
        }

        if (!value){
            Py_DECREF(tuple);
            LOG(FATAL, PY, "Cannot convert argument\n");
            return nullptr;
        }
        // value reference stolen here:
        PyTuple_SetItem(tuple, i, value);
    }
    return tuple;
}

static PyObject* mydeviceOnLoadScreen(PyObject */*self*/, PyObject *args)
{
    if(!PyArg_ParseTuple(args, "O", &_this->_on_load_screen_callback)){
        return PyBool_FromLong(false);
    }
    Py_INCREF(_this->_on_load_screen_callback);
//    _this->setOnLoadScreenCallback(callback);
   return PyBool_FromLong(true);
}

static PyObject* mydeviceGetWidgetTextOnScreenAt(PyObject */*self*/, PyObject *args)
{
    if(!_this->_get_widget_text_on_screen_at_proc){
        return PyUnicode_FromString("");
    }
    int32_t x = -1;
    int32_t y = -1;
    if(!PyArg_ParseTuple(args, "ii", &x, &y)){
        return PyBool_FromLong(false);
    }
    std::string res = _this->_get_widget_text_on_screen_at_proc(x, y);
    return res.size() == 0 ? 
        PyUnicode_FromString("") :
        PyUnicode_FromString(res.c_str());
}

static PyObject* mydeviceTouchScreenAt(PyObject */*self*/, PyObject *args)
{
    if(!_this->_touch_screen_at_point_proc){
        return PyBool_FromLong(false);
    }
    int32_t x = -1;
    int32_t y = -1;
    if(!PyArg_ParseTuple(args, "ii", &x, &y)){
        return PyBool_FromLong(false);
    }
    return PyBool_FromLong(_this->_touch_screen_at_point_proc(x, y));
}

static PyObject* mydeviceLoadScreenById(PyObject */*self*/, PyObject *args)
{
    if(!_this->_load_screen_by_id_proc){
        return PyBool_FromLong(false);
    }
    char *msg = 0;
    if(!PyArg_ParseTuple(args, "s", &msg)){
        return PyBool_FromLong(false);
    }
    std::string str = trim(msg);
    return PyBool_FromLong(_this->_load_screen_by_id_proc(str.c_str()));
}

static PyObject* mydeviceTouchWidgetById(PyObject */*self*/, PyObject *args)
{
    if(!_this->_touch_widget_by_id_proc){
        return PyBool_FromLong(false);
    }
    char *msg = 0;
    if(!PyArg_ParseTuple(args, "s", &msg)){
        return PyBool_FromLong(false);
    }
    std::string str = trim(msg);
    return PyBool_FromLong(_this->_touch_widget_by_id_proc(str.c_str()));
}

static PyObject* mydeviceTouchWidgetByText(PyObject */*self*/, PyObject *args)
{
    if(!_this->_touch_widget_by_text_proc){
        return PyBool_FromLong(false);
    }
    char *msg = 0;
    int32_t nth_obj = 0;
    if(!PyArg_ParseTuple(args, "si", &msg, &nth_obj)){
        return PyBool_FromLong(false);
    }
    
    // std::string str = trim(msg);
    return PyBool_FromLong(_this->_touch_widget_by_text_proc(msg, nth_obj));
}
}//namespace

PythonWrapper::PythonWrapper()
{
    _this = this;
}

PythonWrapper::~PythonWrapper()
{
}

bool PythonWrapper::pythonInit(int argc, char **argv)
{
    auto module_paths = getModulePathsFromJson(SERVICES_JSON);
    module_paths = appendModulePathOfScriptToRun(module_paths, argc, argv);
    _py_script_to_run = getPythonScriptToRun(argc, argv);
    if(!_py_script_to_run.size()) {
        return false;
    }

    setlocale(LC_ALL, "en_US.UTF-8");

    std::string program_name(argv[0]);
    std::wstring w_program_name;
    PyStatus status;
    
    PyConfig_InitPythonConfig(&_config);

    /* Set the program name before reading the configuration
       (decode byte string from the locale encoding).

       Implicitly preinitialize Python. */
    status = PyConfig_SetBytesString(
        &_config, &_config.program_name, program_name.c_str());
    if (PyStatus_Exception(status)) {
        goto done;
    }

    /* Read all configuration at once */
    status = PyConfig_Read(&_config);
    if (PyStatus_Exception(status)) {
        goto done;
    }

    /* Specify sys.path explicitly */
    /* If you want to modify the default set of paths, finish
       initialization first and then use PySys_GetObject("path") */
    _config.module_search_paths_set = 1;
    for(const auto &path : module_paths){
        status = PyWideStringList_Append(&_config.module_search_paths,
                                        path.c_str());
        if (PyStatus_Exception(status)) {
            goto done;
        }
    }

    w_program_name = std::wstring(program_name.begin(), program_name.end());
    /* Override executable computed by PyConfig_Read() */
    status = PyConfig_SetString(&_config, &_config.executable,
                                w_program_name.c_str());//L"/path/to/my_executable");
    if (PyStatus_Exception(status)) {
        goto done;
    }


    PyImport_AppendInittab("mydevice", &pyInitMyModule);
    status = Py_InitializeFromConfig(&_config);
    _initialized = setupStderr();

    return (status.exitcode == 0 && _initialized);

done:
    PyConfig_Clear(&_config);
    return status.exitcode;
}

PyObject *PythonWrapper::pythonCallFunction(
    PyObject *pModule, 
    const char* func, 
    const std::vector<std::string> &args)
{
    PyObject *pFunc = PyObject_GetAttrString(pModule, func);

    if (!pFunc){
        if (PyErr_Occurred()){
            PyErr_Print();
            LOG(FATAL, PY, "%s\n", getStderrText().c_str());
        }
        LOG(DEBUG, PY, "Cannot find function \"%s\"\n", func);
        return nullptr;
    }
    else if (!PyCallable_Check(pFunc)){
        if (PyErr_Occurred()){
            PyErr_Print();
            LOG(FATAL, PY, "%s\n", getStderrText().c_str());
        }
        LOG(DEBUG, PY, "\"%s\" is not a callable function\n", func);
        return nullptr;
    }
    else
    {
        PyObject *tuple = argsToTuple(args);
        PyObject *ret_value = PyObject_CallObject(pFunc, tuple);
        Py_DECREF(tuple);
        Py_DECREF(pFunc);
        if (!ret_value){
            PyErr_Print();
            LOG(FATAL, PY, "%s\n", getStderrText().c_str());
            return nullptr;
        }
        // else{
        //     long val = PyLong_AsLong(ret_value);
        //     LOG(DEBUG, PY, "Result of call: %i\n", val);
        //     Py_DECREF(ret_value);
        // }
        return ret_value;
    }

    return nullptr;
}

long PythonWrapper::pythonCallMain(std::function<void()> app_event_loop)
{
    if(!_py_script_to_run.size()) {
        return 0;
    }
   PyObject *obj = PyUnicode_FromString(_py_script_to_run.c_str());
   _py_module =  PyImport_Import(obj);
   Py_DECREF(obj);

   if(!_py_module || PyErr_Occurred()){
        PyErr_Print();
        LOG(WARNING, PY, "%s\nFailed to load %s\n", getStderrText().c_str(), _py_script_to_run.c_str());
       return false;
   }
    _app_event_loop = app_event_loop;
    const char main_fn[] = "main";
    PyObject *ret = pythonCallFunction(_py_module, main_fn, std::vector<std::string>());
    long ret_val = PyLong_AsLong(ret);
    Py_DECREF(ret);
    Py_DECREF(_py_module);
    _py_module = nullptr;
    return ret_val;
}

bool PythonWrapper::eventLoop()
{
    if(!_py_event_loop_callback ||
        _py_event_loop_callback->ob_refcnt <= 0){
        return true;//there is no python code to execute, just return true
    }

    ScreenLoaded *msg = ipc_recv_screen_loaded_message();

    PyObject *list = PyList_New(1);
         PyObject *dict = PyDict_New();
         if(msg && msg->_screen_id[0]){
            PyDict_SetItem(dict, PyUnicode_FromString("screen_id"), PyUnicode_FromString(msg->_screen_id));
         }
         else {
            PyDict_SetItem(dict, PyUnicode_FromString("screen_id"), PyUnicode_FromString(""));
         }
        //  PyDict_SetItem(dict, PyUnicode_FromString("value"), PyLong_FromDouble(123.));
         PyList_Append(list, dict);
    PyObject *tuple = PyTuple_New(1);
    PyTuple_SetItem(tuple, 0, dict);

    PyObject *ret_value = PyObject_Call(_py_event_loop_callback, tuple, nullptr);
    if (PyErr_Occurred()){
        PyErr_Print();
        LOG(FATAL, PY, "%s\n", getStderrText().c_str());
    }
    Py_DECREF(tuple);
    Py_DECREF(dict);
    Py_DECREF(list);
    Py_DECREF(dict);
    bool ret = PyBool_Check(ret_value) && ret_value == Py_True;
    Py_DECREF(ret_value);
    return ret;
}

void PythonWrapper::onLoadScreenCallback(const char* screen_id)
{
    if(!_on_load_screen_callback ||
        _on_load_screen_callback->ob_refcnt <= 0){
        return;
    }

    PyObject *list = PyList_New(1);
         PyObject *dict = PyDict_New();
         PyDict_SetItem(dict, PyUnicode_FromString("screen_id"), PyUnicode_FromString(screen_id));
        //  PyDict_SetItem(dict, PyUnicode_FromString("value"), PyLong_FromDouble(123.));
         PyList_Append(list, dict);
    PyObject *tuple = PyTuple_New(1);
    PyTuple_SetItem(tuple, 0, dict);

    PyObject *value = PyObject_Call(_on_load_screen_callback, tuple, nullptr);
    if (PyErr_Occurred()){
        PyErr_Print();
        LOG(FATAL, PY, "%s\n", getStderrText().c_str());
    }
    Py_DECREF(tuple);
    Py_DECREF(dict);
    Py_DECREF(list);
    Py_DECREF(dict);
    Py_DECREF(value);

}

void PythonWrapper::registerTouchWidgetProcs(
        std::function <bool(const char* widget_id)> load_screen_by_id_proc,
        std::function <bool(const char* widget_id)> touch_widget_by_id_proc,
        std::function <bool(const char *obj_text, int nth_obj)> touch_widget_by_text_proc,
        std::function <bool(int32_t x, int32_t y)> touch_screen_at_point_proc,
        std::function <std::string(int32_t x, int32_t y)> get_widget_text_on_screen_at_proc)
{
    _load_screen_by_id_proc = load_screen_by_id_proc;
    _touch_widget_by_id_proc = touch_widget_by_id_proc;
    _touch_widget_by_text_proc = touch_widget_by_text_proc;
    _touch_screen_at_point_proc = touch_screen_at_point_proc;
    _get_widget_text_on_screen_at_proc = get_widget_text_on_screen_at_proc;
}
