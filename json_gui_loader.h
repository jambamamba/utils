#pragma once

#include <lvgl/lvgl.h>
#include <cJSON_Utils.h>
#include <json_utils.h>
#include <optional>

typedef enum SCREEN_TYPE{
    NO_SCREEN,
    OPENING_MENU1,
    OPENING_MENU2,
    REOPEN_JOBS,
    MENU_SETUP,
    WRITERS_MENU,
    STENO_STROKES,
    DATE_TIME,
    PAGE1,
    PAGE2,
    SENSORBOARD_KEYSTROKE,
    FUNCTION_KEYSTROKE,
    MAIN_STATUS,
    AUDIO_SETUP,
    SETUP_BACKLIGHT,
    SETUP_OPTIONS,
    KEYBOARD_ADJUST,
    SETUP_TRANSLATIONS,
    SETUP_WIRELESS_WIFI,
    DISPLAY_FONT,
    SETUP_DATE_TIME,
    MAIN_SETUP_DISPLAY_COLOR,
    KEYBOARDLAYOUT,
    SOFTWAREUPDATE,
    SERVICE_REMINDER,
    WIRELESS_MENU,
    MENU_SETUP2,
    WIFI_SEARCHINFO,
    WIFI_OPTIONS_MANAGER,
    WIFI_CONNECT_SCREEN,
    WIFI_INFO,
    WIFI_OPTIONS,
    WIFI_OPTIONS_SETTINGS,
    LCD_CALIBRATION,
    DIAGNOSTICS,
    JOB_STATS,
    STORAGE,
    BLUETOOTH,
    BATTERY_INFO,
    LOAD_DICTIONARY,
    SETUP_DICTIONARY,
    DISPLAY_MENU,
    TEST_MODE,
    KEYBOARD_MENU,
    KEYBOARD_PROFILE,
    JDEFINES_OPTIONS,
    ABOUT_PAGE,
    SCREEN_MAX_NUM,
}SCREEN_TYPE_E;

lv_obj_t* createScreen(SCREEN_TYPE_E screen_id);
SCREEN_TYPE_E screenIdToEnum(const char *screen_id);
const char* screenEnumToString(SCREEN_TYPE_E screen_type);
void loadGuiJsonFile(const char *file_name);
void loadScreenById(const char *id);
void unloadScreen();
void deleteGuiJson();
void loadSetupScreen(const char *screen_id);
void handleScreenClose(const char* json);
void addButton(lv_obj_t *screen, cJSON *json_parent);
bool touchWidgetById(const char* obj_id);
bool touchWidgetByText(const char *obj_text, int nth_obj);
bool touchScreenAtPoint(int32_t x, int32_t y);
std::string getWidgetTextOnScreenAt(int32_t x, int32_t y);
std::optional<lv_area_t> getObjectRect(const lv_obj_t *obj, int depth = 0);
lv_obj_t *drawCirclOnWidget(const lv_area_t &pos);
void sendWidgetTouchEvent(const lv_obj_t *obj, lv_event_code_t event_code/*LV_EVENT_CLICKED|LV_EVENT_RELEASED*/);
void jsonGuiEventLoop();
