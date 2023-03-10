#include "title_bar.h"
#include "json_gui_loader.h"
#include "screen_manager.h"
#include <stdio.h>
#include <stdlib.h>

// Refer https://docs.lvgl.io/master/overview/font.html#add-new-symbols, for how to add new symbols to project
LV_FONT_DECLARE(title_bar_icons);

#define WRENCH_SYMBOL "\xEF\x82\xAD"     //0xf0ad
#define CHARGING_SYMBOL "\xEF\x87\xA6"   //0xf1e6
#define MICROPHONE_ON "\xEF\x8F\x89"     //0xf3c9
#define MICROPHONE_OFF "\xEF\x94\xB9"   //0xf539
#define NETWORK_COVERAGE "\xEF\x80\x92" //0xf012

static lv_obj_t* add_bar(
    lv_obj_t *parent,
    int pos_x,
    int width,
    int height,
    lv_style_t *bg,
    lv_style_t *indicator
    ){    
    lv_obj_t *bar_obj = lv_bar_create(parent);
    lv_obj_align(bar_obj, LV_ALIGN_LEFT_MID, pos_x, 0);
    lv_obj_set_size(bar_obj, width, height);

    lv_obj_add_style(bar_obj, bg, LV_PART_MAIN);
    lv_obj_add_style(bar_obj, indicator, LV_PART_INDICATOR);
    return bar_obj;
}

static void delete_style(lv_event_t *e){
    lv_style_t *style = (lv_style_t*)e->user_data;
    delete style;
}

static lv_obj_t* add_bar_with_styles(
    lv_obj_t *parent,
    int pos_x,
    int width,
    int height,
    lv_color_t color,
    void(*callback)(lv_event_t*)
    ){
    static lv_style_t bg;
    lv_style_init(&bg);
    lv_style_set_bg_color(&bg, lv_color_hex(0xffffff));
    lv_style_set_bg_opa(&bg, LV_OPA_COVER);
    lv_style_set_radius(&bg, 0);
    lv_style_set_border_width(&bg, 0);

    lv_style_t *indicator = (lv_style_t*)malloc(sizeof(lv_style_t));
    lv_style_init(indicator);
    lv_style_set_bg_color(indicator, color);
    lv_style_set_bg_opa(indicator, LV_OPA_COVER);
    lv_style_set_radius(indicator, 0);
    lv_style_set_border_width(indicator, 0);

    lv_obj_t *bar =  add_bar(parent, pos_x, width, height, &bg, indicator);
    lv_bar_set_value(bar, 90, LV_ANIM_OFF);
    lv_obj_add_flag(bar, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(bar, callback, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(bar, delete_style, LV_EVENT_DELETE, (void*)indicator);
    return bar;
}

static void battery_callback(lv_event_t *e){
    ScreenManager::getScreenManager()->loadScreenByScreenEnum(MAIN_STATUS);
}

static void audio_callback(lv_event_t *e){
    ScreenManager::getScreenManager()->loadScreenByScreenEnum(AUDIO_SETUP);
}

static void sd_card_callback(lv_event_t *e){
    ScreenManager::getScreenManager()->loadScreenByScreenEnum(REOPEN_JOBS);
}

static void wireless_menu_callback(lv_event_t *e){
    ScreenManager::getScreenManager()->loadScreenByScreenEnum(WIRELESS_MENU);
}

static void service_reminder_callback(lv_event_t *e){
    ScreenManager::getScreenManager()->loadScreenByScreenEnum(SERVICE_REMINDER);
}

static void add_strokes(
    lv_obj_t *parent,
    int width,
    int height
    ){
    static lv_style_t text_color;
    lv_style_init(&text_color);
    lv_style_set_text_color(&text_color, lv_color_hex(0xffffff));
    lv_obj_t *strokes = lv_label_create(parent);
    lv_label_set_text(strokes, "Strokes : 0");
    lv_obj_align(strokes, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(strokes, width, height);
    lv_obj_add_style(strokes, &text_color, LV_PART_MAIN);
}

static void add_pages(lv_obj_t *parent, int width, int height){
    static lv_style_t text_color;
    lv_style_init(&text_color);
    lv_style_set_text_color(&text_color, lv_color_hex(0xffffff));
    lv_obj_t *pages = lv_label_create(parent);
    lv_label_set_text(pages, "Pages : 0");
    lv_obj_align(pages, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_size(pages, width, height);
    lv_obj_add_style(pages, &text_color, LV_PART_MAIN);
}

static void add_icon(lv_obj_t *parent,
                    int pos_x,
                    int width,
                    int height,
                    bool custom_font,            // false: if built in font; true: if added externally
                    const void* symbol,
                    void(*callback)(lv_event_t*))
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, width, height);
    lv_obj_align(btn, LV_ALIGN_LEFT_MID, pos_x, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_10, 0);
    if(custom_font){
        lv_obj_set_style_text_font(btn, &title_bar_icons, 0);
    }
    lv_obj_set_style_bg_img_src(btn, symbol, 0);
    lv_obj_add_event_cb(btn, callback, LV_EVENT_CLICKED, NULL);

    //to change network coverage color if turned off : start
    if(symbol==NETWORK_COVERAGE){
        char *data_json_string = loadJsonFromFile("wifi_options.json");
        if (data_json_string == nullptr){
            return;
        }
        cJSON* data_json = cJSON_Parse(data_json_string);
        if (data_json == nullptr){

            const char *error_ptr = cJSON_GetErrorPtr();
            
            if (error_ptr != nullptr){
                return;
            }
        }
        free(data_json_string);       
        if(cJSON_GetObjectItemCaseSensitive(data_json, "WiFi On:") && strcmp(cJSON_GetObjectItemCaseSensitive(data_json, "WiFi On:")->valuestring,"Off")==0){
            lv_obj_set_style_text_color(btn, lv_palette_main(LV_PALETTE_GREY), 0);
        }
    }
    //End
}

void add_title_bar(lv_obj_t *screen){
    static lv_style_t bar_style;
    lv_style_init(&bar_style);
    lv_obj_t *title_bar = lv_obj_create(screen);
    lv_obj_set_size(title_bar, LV_HOR_RES_MAX, 30);
    lv_obj_set_pos(title_bar, 0,0);
    lv_obj_clear_flag(title_bar, LV_OBJ_FLAG_SCROLLABLE);

    lv_style_set_bg_color(&bar_style, lv_color_hex(0x003f62));
    lv_style_set_bg_opa(&bar_style, LV_OPA_COVER);
    lv_style_set_radius(&bar_style, 0);
    lv_style_set_border_width(&bar_style, 0);
    lv_obj_add_style(title_bar, &bar_style, LV_PART_MAIN);

    add_icon(title_bar, 0, 10, 10, true, CHARGING_SYMBOL, battery_callback);
    lv_obj_t *battery_bar = add_bar_with_styles(title_bar, 20, 40, 10, lv_palette_main(LV_PALETTE_LIGHT_GREEN), battery_callback);
    add_icon(title_bar, 70, 10, 10, false, LV_SYMBOL_SD_CARD, sd_card_callback);
    lv_obj_t *sd_card_bar = add_bar_with_styles(title_bar, 90, 50, 10, lv_palette_main(LV_PALETTE_INDIGO), sd_card_callback);
    add_icon(title_bar, 150, 10, 10, true, MICROPHONE_ON, audio_callback);
    lv_obj_t *audio_bar = add_bar_with_styles(title_bar, 170, 100, 10, lv_palette_main(LV_PALETTE_GREY), audio_callback);
    add_icon(title_bar, 450, 10, 10, true, NETWORK_COVERAGE, wireless_menu_callback);
    add_icon(title_bar, 500, 10, 10, true, WRENCH_SYMBOL, service_reminder_callback);
    add_strokes(title_bar, 100, 20);
    add_pages(title_bar, 70, 20);
}
