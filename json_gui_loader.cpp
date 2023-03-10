#include "json_gui_loader.h"

#include <lvgl/lvgl.h>
#include <lvgl/src/draw/lv_draw_rect.h>
#include <lvgl/src/widgets/canvas/lv_canvas.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json_utils.h>
#include <debug_logger.h>
#include <time.h>

#include <file_change_notifier.h>
#include <language_translation.h>
#include <ui/screen_manager.h>
#include <ui/screen_button.h>
#include <ui/device_event_queue.h>
#include <simple_timer.h>

LOG_CATEGORY(JSON, "JSON")

#define MONITOR_HOR_RES 800 //osm todo
#define MONITOR_VER_RES 600 //osm todo

static cJSON *gui_json_handle = NULL; 
static struct _lv_timer_t *timer = nullptr;
static std::map<std::string /*id*/, lv_obj_t*> _widget_map;
extern "C" lv_disp_drv_t _display_driver;
extern "C" lv_disp_t *_display;

static DeviceEventQueue _event_queue;

typedef void (*update_timer_function_t)(struct _lv_timer_t *);
static update_timer_function_t getUpdateTimerFunction(const char* function_name) 
{
    //todo: use reflection for this: // #include <dlfcn.h>
    #if 0
    if(strcmp("update_steno_strokes", function_name) == 0) {
        return update_steno_strokes;
    }
    else if(strcmp("update_steno_translations", function_name) == 0) {
        return update_steno_translations;
    }
    else if(strcmp("update_main_status", function_name) == 0) {
        return update_main_status;
    }
    else if(strcmp("Full_Screen_Image", function_name) == 0){
        return Full_Screen_Image; 
    }
    else if(strcmp("update_setup_options", function_name) == 0) {
        return update_setup_options;
    }    
    else if(strcmp("update_backlight", function_name) == 0) {
        return update_backlight;
    }    
    else if(strcmp("update_keyboard_adjust", function_name) == 0) {
        return update_keyboard_adjust;
    } 
    else if(strcmp("update_keyboard_layout_status", function_name) == 0){
        return update_keyboard_layout_status;
    }
    else if(strcmp("software_update_status", function_name) == 0){
        // return software_update_status;
    }
    else if(strcmp("update_menu_setup2", function_name) == 0){
        return update_menu_setup2;
    }
    else if(strcmp("wifi_search", function_name) == 0){
        return WiFiSearch::update_wifi_window;
    }else if(strcmp("wifi_options_manager", function_name) == 0){
        return WifiOptionsManager::update_wifi_manage_window;
    }    
    #endif//0
    LOG(FATAL, JSON, "Timer function is not implemented in code: '%s'\n", function_name);
    return NULL;
}

static lv_obj_t *addWindow(lv_obj_t *screen, cJSON *json_screen)
{
    cJSON *json_window = cJSON_GetObjectItemCaseSensitive(json_screen, "window");
    if (!json_window || !json_window->string) {
        return NULL;
    }
    cJSON *json_header_height = cJSON_GetObjectItemCaseSensitive(json_window, "header_height");
    int header_height = (json_header_height && cJSON_IsString(json_header_height)) ? atoi(json_header_height->valuestring) : 32;
    
    int width = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_window, "width"), lv_obj_get_width(screen));
    int height = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_window, "height"), lv_obj_get_height(screen));
    int background_color = colorFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_window, "background"), 0x003f62);
    lv_color_t bg_color = {.full = (uint32_t)background_color};

    lv_obj_t *window = lv_win_create(screen, header_height);
    lv_obj_set_size(window, width, height);
    cJSON *json_window_title = cJSON_GetObjectItemCaseSensitive(json_window, "title");
    if (json_window_title && cJSON_IsString(json_window_title)) {
        lv_obj_t* title = lv_win_add_title(window, json_window_title->valuestring);
        // lv_obj_add_style(title, &bg_style, LV_PART_MAIN);
    }
    cJSON *json_id = cJSON_GetObjectItemCaseSensitive(json_window, "id");
    char *id = (json_id && cJSON_IsString(json_id)) ? json_id->valuestring : 0;

    cJSON *json_value = cJSON_GetObjectItem(json_window, "update_timer");
    cJSON *child_json_value = (json_value && cJSON_IsObject(json_value)) ? cJSON_GetObjectItem(json_value, "function") : NULL;
    char *update_timer_function = (child_json_value && cJSON_IsString(child_json_value)) ? child_json_value->valuestring : NULL;
    double update_hz = json_value ? doubleFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_value, "update_hz"), 0) : 0;

    if(update_timer_function && *update_timer_function) {
        int update_interval_ms = 1000. / update_hz;
        if (timer !=  nullptr) {
            lv_timer_del(timer);
        }
        timer = lv_timer_create(
            getUpdateTimerFunction(update_timer_function), 
            update_interval_ms, 
            window);
        timer->user_data = window;
    }

    return window;
}

void addButton(lv_obj_t *screen, cJSON *json_parent)
{
    cJSON *json_value = nullptr;
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "id");
    const char *id = (json_value && cJSON_IsString(json_value)) ? json_value->valuestring : 0;
    int x = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "x"), LV_HOR_RES_MAX);
    int y = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "y"), LV_VER_RES_MAX);
    int width = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "width"), LV_HOR_RES_MAX);
    int height = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "height"), LV_VER_RES_MAX);
    int background_color = colorFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "background"), 0xff409ff3);
    int border_color = colorFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "border"), 0xff000000);
    int gradient_color = colorFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "gradient"), 0xff1efcff);
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "label");
    const char *label = (json_value && cJSON_IsString(json_value)) ? json_value->valuestring : NULL;
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "on_press");
    char *on_press = (json_value) ? cJSON_IsString(json_value) ? strdup(json_value->valuestring) : cJSON_IsObject(json_value) ? cJSON_PrintUnformatted(json_value)
                                                                                                                              : NULL
                                  : NULL;
    lv_obj_t *obj = create_button(screen, id, x, y, width, height, label, background_color, border_color, gradient_color, on_press);
    _widget_map[id] = obj;
    if (on_press){
        free(on_press);
    }
}

static void addSwitch(lv_obj_t *screen, cJSON *json_parent)
{
    cJSON *json_value = NULL;
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "id");
    char *id = (json_value && cJSON_IsString(json_value)) ? json_value->valuestring : 0;
    int x = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "x"), LV_HOR_RES_MAX);
    int y = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "y"), LV_VER_RES_MAX);
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "label");
    char *label = (json_value && cJSON_IsString(json_value)) ? json_value->valuestring : NULL;
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "on_press");
    char *on_press = (json_value) ? cJSON_IsString(json_value) ? strdup(json_value->valuestring) : cJSON_IsObject(json_value) ? cJSON_PrintUnformatted(json_value)
                                                                                                                              : NULL
                                  : NULL;
    lv_obj_t *obj = create_switch(screen, id, x, y, label, on_press);
    _widget_map[id] = obj;
    if (on_press){
        free(on_press);
    }
}

static void addCheckbox(lv_obj_t *screen, cJSON *json_parent)
{
    cJSON *json_value = NULL;
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "id");
    char *id = (json_value && cJSON_IsString(json_value)) ? json_value->valuestring : 0;
    int x = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "x"), LV_HOR_RES_MAX);
    int y = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "y"), LV_VER_RES_MAX);
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "label");
    char *label = (json_value && cJSON_IsString(json_value)) ? json_value->valuestring : NULL;
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "on_press");
    char *on_press = (json_value) ? cJSON_IsString(json_value) ? strdup(json_value->valuestring) : cJSON_IsObject(json_value) ? cJSON_PrintUnformatted(json_value)
                                                                                                                              : NULL
                                  : NULL;
    lv_obj_t *obj = create_checkbox(screen, id, x, y, label, on_press);
    _widget_map[id] = obj;
    if (on_press){
        free(on_press);
    }
}

static char **createStringArray(const cJSON *json_array, int max_string_sz)
{
    int num_items = cJSON_IsArray(json_array) ? cJSON_GetArraySize(json_array) : 0;
    if (num_items <= 0){
        return nullptr;
    }
    char **items = (char **)malloc((num_items + 1) * sizeof(char **));
    int i;
    for (i = 0; i < num_items; ++i){
        cJSON *json_label = cJSON_GetArrayItem(json_array, i);
        if (json_label && cJSON_IsString(json_label)){
            char *label = json_label->valuestring;
            if (label && *label){
                if (strlen(label) > max_string_sz){
                    LOG(FATAL, JSON, "The label is too big, only %i characters allowed: '%s'\n", max_string_sz, label);
                }
                items[i] = strdup(label);
            }
            else{
                items[i] = strdup("");
            }
        }
    }
    items[i] = NULL;
    return items;
}

static void deleteStringArray(char **array)
{
    for (int i = 0; array[i]; ++i){
        free(array[i]);
    }
    free(array);
}

static void addChart(lv_obj_t *screen, cJSON *json_parent)
{
    cJSON *json_value = NULL;
    cJSON *child_json_value = NULL;
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "id");
    char *id = (json_value && cJSON_IsString(json_value)) ? json_value->valuestring : 0;
    int x = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "x"), LV_HOR_RES_MAX);
    int y = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "y"), LV_VER_RES_MAX);
    int width = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "width"), LV_HOR_RES_MAX);
    int height = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "height"), LV_VER_RES_MAX);
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "type");
    char *type = (json_value && cJSON_IsString(json_value)) ? json_value->valuestring : 0;

    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "xlabels");
    char **xlabels = createStringArray(json_value, 32);
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "ylabels");
    char **ylabels = createStringArray(json_value, 32);

    int ymin = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "ymin"), LV_VER_RES_MAX);
    int ymax = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_parent, "ymax"), LV_VER_RES_MAX);
    json_value = cJSON_GetObjectItemCaseSensitive(json_parent, "series");
    char **series = createStringArray(json_value, 32);

    json_value = cJSON_GetObjectItem(json_parent, "update_timer");
    child_json_value = (json_value && cJSON_IsObject(json_value)) ? cJSON_GetObjectItem(json_value, "function") : NULL;
    char *update_timer_function = (child_json_value && cJSON_IsString(child_json_value)) ? child_json_value->valuestring : NULL;
    int update_hz = intFromJsonValue(cJSON_GetObjectItemCaseSensitive(json_value, "update_hz"), 0);

    lv_obj_t *obj = create_chart(screen, id, x, y, width, height, type, xlabels, ylabels, ymin, ymax, series, update_timer_function, update_hz);
    _widget_map[id] = obj;

    // osm todo: do deletion when chart is destroyed, dont do it now
    //  deleteStringArray(xlabels);
    //  deleteStringArray(ylabels);
    //  deleteStringArray(series);
}

static const char* const _screen_json_id[SCREEN_MAX_NUM] = {
    [NO_SCREEN] = "none",
    [OPENING_MENU1] = "opening_menu1",
    [OPENING_MENU2] = "opening_menu2",
    [REOPEN_JOBS] = "reopen_jobs",
    [MENU_SETUP]= "menu_setup",
    [WRITERS_MENU] = "writers_menu",
    [STENO_STROKES] = "page/steno-strokes",
    [DATE_TIME] = "page/date-time",
    [PAGE1] = "page1",
    [PAGE2] = "page2",
    [SENSORBOARD_KEYSTROKE] = "sensorboard-keystroke",
    [FUNCTION_KEYSTROKE] = "functionkey-keystroke",
    [MAIN_STATUS] = "main_status",
    [AUDIO_SETUP] = "audio_setup", 
    [SETUP_BACKLIGHT] = "backlight",
    [SETUP_OPTIONS] = "setup_options",
    [KEYBOARD_ADJUST] = "keyboard_adjust",
    [SETUP_TRANSLATIONS] = "setup_translations",
    [SETUP_WIRELESS_WIFI] = "wifi_options",
    [DISPLAY_FONT] = "displayfont",
    [SETUP_DATE_TIME] = "date_time",
    [MAIN_SETUP_DISPLAY_COLOR] = "main_setup_display_color",
    [KEYBOARDLAYOUT] = "keyboardlayout",
    [SOFTWAREUPDATE] = "softwareupdate",
    [SERVICE_REMINDER] = "service_reminder",
    [WIRELESS_MENU] = "wireless_menu",
    [MENU_SETUP2] = "menu_setup2",
    [WIFI_SEARCHINFO] = "wifi_search",
    [WIFI_OPTIONS_MANAGER] = "wifi_options_manager",
    [WIFI_CONNECT_SCREEN] = "wifi_connect",
    [WIFI_INFO] = "wifi_info",
    [WIFI_OPTIONS] = "wifi_options",
    [WIFI_OPTIONS_SETTINGS] = "wifi_options_settings",
    [LCD_CALIBRATION] = "Full_Screen_Image",
    [DIAGNOSTICS] = "diagnostics",
    [JOB_STATS] = "job_stats",
    [STORAGE] = "storage",
    [BLUETOOTH] = "bluetooth",
    [BATTERY_INFO] = "battery_info",
    [LOAD_DICTIONARY] = "load_dictionary",
    [SETUP_DICTIONARY] = "setup_dictionaries",
    [DISPLAY_MENU] = "display_menu",
    [TEST_MODE] = "test_mode",
    [KEYBOARD_MENU] = "keyboard_menu",
    [KEYBOARD_PROFILE] = "keyboard_profile",
    [JDEFINES_OPTIONS] = "jdefines_options",
    [ABOUT_PAGE] = "about_page",
};

const char* screenEnumToString(SCREEN_TYPE_E screen_id)
{
    if(screen_id >= NO_SCREEN && screen_id < SCREEN_MAX_NUM){
        return _screen_json_id[screen_id];
    }
    LOG(FATAL, JSON, "Screen enum out of range, screen_id:%i\n", screen_id);
    return nullptr;
}

SCREEN_TYPE_E screenIdToEnum(const char *screen_id)
{
    for(int i = NO_SCREEN; i < SCREEN_MAX_NUM; ++i){
        if(strcmp(_screen_json_id[i], screen_id) == 0){
            return static_cast<SCREEN_TYPE_E>(i);
        }
    }
    LOG(WARNING, JSON, "Screen id not found: %s\n", screen_id);//TODO: chnage this to fatal after checking why we get so many invalid id's
    return NO_SCREEN;
}

void loadGuiJsonFile(const char *file_name)
{    
    char *gui_json_string = loadJsonFromFile(file_name);
    if (!gui_json_string) {
        return;
    }

    gui_json_handle = cJSON_Parse(gui_json_string);
    if (gui_json_handle == NULL)  {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            LOG(FATAL, JSON, "Error before: %s\n", error_ptr);
        }
    }

    cJSON *version = cJSON_GetObjectItemCaseSensitive(gui_json_handle, "version");
    if (cJSON_IsNumber(version) && (version->valuedouble != 1.0)) {
        LOG(WARNING, JSON, "Unsupported json version \"%f\" - only 1.0 is acceptable\n", version->valuedouble);
    }
    free(gui_json_string);
}

void deleteGuiJson()
{
    if(gui_json_handle){
        cJSON_Delete(gui_json_handle);
    }
}

static lv_obj_t *createCanvas(lv_obj_t *parent)
{
    lv_obj_t *canvas_obj = lv_canvas_create(parent);
    // lv_canvas_t *canvas = ((lv_canvas_t *)canvas);
    lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(MONITOR_HOR_RES, MONITOR_VER_RES)];
    lv_canvas_set_buffer(canvas_obj, cbuf, MONITOR_HOR_RES, MONITOR_VER_RES, LV_IMG_CF_TRUE_COLOR_ALPHA);
    lv_obj_align(canvas_obj, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_fill_bg(canvas_obj, lv_color_hex(0xff0000), 0x00);
    return canvas_obj;
}

lv_obj_t* 
createScreen(SCREEN_TYPE_E screen_id)
{
    if(gui_json_handle == NULL){
        LOG(FATAL, JSON, "Json file handle not created")
    }
    bool screen_id_found = false;
    cJSON *json_screens = cJSON_GetObjectItemCaseSensitive(gui_json_handle, "screens");
    cJSON *json_screen = NULL;
    const char *screen_id_string = screenEnumToString(screen_id);

    lv_obj_t *screen = lv_obj_create(nullptr);
    lv_obj_set_user_data(screen, (void*)screen_id);
    cJSON_ArrayForEach(json_screen, json_screens) {
        if(strcmp(screen_id_string, cJSON_GetObjectItemCaseSensitive(json_screen, "id")->valuestring) == 0){
            screen_id_found = true;
            lv_obj_t *window = addWindow(screen, json_screen);
            cJSON *current_element = json_screen->child;
            while (current_element != NULL){
                if (current_element->string && strcmp("id", current_element->string) == 0){
                }
                else if (strcmp("button", current_element->string) == 0){
                    addButton(screen, current_element);
                }
                else if (strcmp("switch", current_element->string) == 0){
                    addSwitch(screen, current_element);
                }
                else if (strcmp("checkbox", current_element->string) == 0){
                    addCheckbox(screen, current_element);
                }
                else if (strcmp("chart", current_element->string) == 0){
                    addChart(screen, current_element);
                }
                current_element = current_element->next;
            }
            break;
        }
    }
    if(!screen_id_found){
        LOG(FATAL, JSON, "Screen id %s is not valid\n", screen_id);
    }
    return screen;
}

void loadScreenById(const char* screen_id)
{
    ScreenManager::getScreenManager()->loadScreen(lv_scr_act(), 
        screenIdToEnum(screen_id));
}

void unloadScreen()
{
    lv_obj_t* s = lv_scr_act();
    if(s != NULL){
        lv_obj_del(s);
    }
}

void loadSetupScreen(const char *json)
{
    if (!json || !*json){
        return;
    }
    cJSON *json_obj = cJSON_Parse(json);
    if (json_obj == NULL){
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL){
            LOG(WARNING, JSON, "Error before: '%s'\n", error_ptr);
        }
        return;
    }
    cJSON *args_json = cJSON_GetObjectItemCaseSensitive(json_obj, "args");
    if(args_json){
        ScreenManager::getScreenManager()->loadScreen(lv_scr_act(), 
            screenIdToEnum(args_json->valuestring));
    }
}

void handleScreenClose(const char* json)
{
    ScreenManager::getScreenManager()->unLoadScreen();
}

lv_obj_t *drawCirclOnWidget(const lv_area_t &pos)
{
    lv_draw_arc_dsc_t dsc = {0};
    lv_draw_arc_dsc_init(&dsc);
    dsc.color = lv_color_hex(0xffff00);
    dsc.width = 100;
    dsc.opa = 0x88;
    lv_obj_t *canvas = createCanvas(lv_scr_act());
    lv_coord_t radius = 30;
    int32_t start_angle = 0;
    int32_t end_angle = 360;
    int32_t x = pos.x1 + (pos.x2 - pos.x1)/2;
    int32_t y = pos.y1 + (pos.y2 - pos.y1)/2;
    lv_canvas_draw_arc(canvas, x, y, radius, start_angle, end_angle, &dsc);
    return canvas;
}

void sendWidgetTouchEvent(const lv_obj_t *obj, lv_event_code_t event_code/*LV_EVENT_CLICKED|LV_EVENT_RELEASED*/)
{
    if((obj->flags & LV_OBJ_FLAG_CLICKABLE) == LV_OBJ_FLAG_CLICKABLE){
        lv_event_send(const_cast<lv_obj_t *>(obj), event_code, nullptr);
    }
}
static bool isObjectWithMatchingText(const lv_obj_t *obj, const char* needle)
{
    if(!needle || !*needle){
        return false;
    }
    const lv_obj_class_t *obj_class = lv_obj_get_class(obj); 
    if(obj_class == &lv_label_class){
        const char * txt = lv_label_get_text(obj);
        if(txt && *txt){
            if(strstr(txt, needle)){
                if(strcmp(txt, needle) == 0){
                    return true;
                }
                else {
                    LOG(WARNING, JSON, "isObjectWithMatchingText: '%s' is contained in '%s', but not exact match\n", needle, txt);
                }
            }
        }
    }
    return false;
}

static int area(const lv_obj_t *obj)
{
    lv_area_t area;
    lv_obj_get_coords(obj, &area);
    return (area.x2 - area.x1) * (area.y2 - area.y1);
}

std::string getWidgetTextOnScreenAt(int32_t x, int32_t y)
{
    //todo: fix this:p
    auto objs = ScreenManager::getScreenManager()->findObjectsAtPosition(x, y);
    if(objs.size() == 0){
        return "";
    }
    lv_obj_t *smallest_area_obj = nullptr;
    for(const lv_obj_t *obj: objs){
        if(!lv_obj_is_valid(obj)){
            continue;
        }
        
        char *text = nullptr;
        if(lv_obj_get_class(obj) == &lv_label_class){
            text = lv_label_get_text(obj);
        }
        else if(lv_obj_get_class(obj) == &lv_list_class){
            text = lv_label_get_text(obj);
        }
        else if(lv_obj_get_class(obj) == &lv_list_text_class){
            text = lv_label_get_text(obj);
        }
        else if(lv_obj_get_class(obj) == &lv_textarea_class){
            text = lv_label_get_text(obj);
        }

        if(text){
            if(!smallest_area_obj){
                smallest_area_obj = (lv_obj_t *)obj;
            }
            else if(area(obj) < area(smallest_area_obj)){
                smallest_area_obj = (lv_obj_t *)obj;
            }
        }

    }
    if(smallest_area_obj){
        char *text = lv_label_get_text(smallest_area_obj);
        return text;
    }
    return "";
}

bool touchScreenAtPoint(int32_t x, int32_t y)
{
    _event_queue.pushEventTouchScreenAtPoint(x, y);    
    return true;
}

bool touchWidgetByText(const char *obj_text, int nth_obj)
{
    const char* obj_name = nullptr;
    std::vector<lv_obj_t *> objs;
    if(obj_text && *obj_text){
        obj_name = obj_text;
        objs = ScreenManager::getScreenManager()->findObjectsByLabelText(obj_text);
    }

    if(objs.size() == 0){
        LOG(FATAL, JSON, "No such widget found: '%s'\n", obj_name);
        return false;
    }
    if(nth_obj < 0 || nth_obj >= objs.size()){
        LOG(FATAL, JSON, "nth object index is invalid: '%s', n:%i\n", obj_name, nth_obj);
        return false;
    }
    _event_queue.pushEventTouchWidgetById(objs.at(nth_obj));    
    return true;
}

bool touchWidgetById(const char* obj_id)
{
    const char* obj_name = nullptr;
    std::vector<lv_obj_t *> objs;
    if(obj_id && *obj_id){
        obj_name = obj_id;
        const auto &it = _widget_map.find(obj_id);
        if(it == _widget_map.end()){
            return false;
        }
        objs.push_back(it->second);
    }

    if(objs.size() == 0){
        LOG(FATAL, JSON, "No such widget found: '%s'\n", obj_name);
        return false;
    }
    _event_queue.pushEventTouchWidgetById(objs.at(0));
    return true;
}

void jsonGuiEventLoop()
{
    _event_queue.handleDeviceEvent();
}
