
#include "screen_manager.h"

#include <debug_logger.h>
#include <map>
#include <sys/time.h>
// #include <mq_utils.h>
#include <vector>

LOG_CATEGORY(ScreenManager, "ScreenManager")

namespace {

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
                    LOG(WARNING, ScreenManager, "isObjectWithMatchingText: '%s' is contained in '%s', but not exact match\n", needle, txt);
                }
            }
        }
    }
    return false;
}

static bool isPositionInsideRect(int32_t x, int32_t y, const std::optional<lv_area_t> &rect)
{
    return rect && 
        x >= rect->x1 &&
        x <= rect->x2 &&
        y >= rect->y1 &&
        y <= rect->y2;
}
static lv_obj_tree_walk_res_t objTreeWalkCb(struct _lv_obj_t * obj, void *objects_)
{
    auto &lv_objects = *(std::map<_lv_obj_t *, std::vector<_lv_obj_t *>>*)objects_;
    lv_objects[lv_obj_get_parent(obj)].push_back(obj);
    //to get all classes:
    //cd /opt/usr_data/sdk/sysroots/aarch64-fslc-linux/usr/src/debug/nextgen-linux/0.1-r0/gui/libs/lvgl 
    // grep -nr "const lv_obj_class_t lv" * --include=*.c
    if(lv_obj_get_class(obj) == &lv_label_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_label_class, text %s\n", lv_label_get_text(obj));
    }
    else if(lv_obj_get_class(obj) == &lv_btn_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_btn_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_slider_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_slider_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_dropdown_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_dropdown_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_dropdownlist_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_dropdownlist_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_bar_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_bar_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_table_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_table_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_switch_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_switch_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_line_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_line_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_checkbox_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_checkbox_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_spinner_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_spinner_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_msgbox_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_msgbox_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_keyboard_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_keyboard_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_menu_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_menu_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_spinbox_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_spinbox_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_chart_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_chart_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_tabview_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_tabview_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_list_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_list_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_list_btn_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_list_btn_class\n");
    }
    else if(lv_obj_get_class(obj) == &lv_list_text_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_list_text_class, text %s\n", lv_label_get_text(obj));
    }
    else if(lv_obj_get_class(obj) == &lv_textarea_class){
        LOG(DEBUG, ScreenManager, "@@@@@ lv_textarea_class\n");
    }
    else {
        LOG(DEBUG, ScreenManager, "@@@@@ unknown class\n");
    }
    return LV_OBJ_TREE_WALK_NEXT;
// lv_arc_class
// lv_templ_class
// lv_canvas_class
// lv_label_class
// lv_slider_class
// lv_dropdown_class
// lv_dropdownlist_class
// lv_bar_class
// lv_table_class
// lv_switch_class
// lv_btn_class
// lv_btnmatrix_class
// lv_textarea_class
// lv_line_class
// lv_img_class
// lv_checkbox_class
// lv_roller_class
// lv_roller_label_class
// lv_spinner_class
// lv_msgbox_class
// lv_msgbox_content_class
// lv_msgbox_backdrop_class
// lv_colorwheel_class
// lv_meter_class
// lv_keyboard_class
// lv_menu_class
// lv_menu_page_class
// lv_menu_cont_class
// lv_menu_section_class
// lv_menu_separator_class
// lv_menu_sidebar_cont_class
// lv_menu_main_cont_class
// lv_menu_main_header_cont_class
// lv_menu_sidebar_header_cont_class
// lv_spangroup_class
// lv_led_class
// lv_calendar_header_dropdown_class
// lv_calendar_header_arrow_class
// lv_calendar_class
// lv_spinbox_class
// lv_chart_class
// lv_tabview_class
// lv_win_class
// lv_imgbtn_class
// lv_tileview_class
// lv_tileview_tile_class
// lv_list_class
// lv_list_btn_class
// lv_list_text_class
// lv_animimg_class
// lv_ffmpeg_player_class
// lv_rlottie_class
// lv_gif_class
// lv_qrcode_class
// obj_class

}

}//namespace

ScreenManager::ScreenManager()
    : _current_screen(NO_SCREEN, nullptr)   
{
}

ScreenManager* ScreenManager::getScreenManager(){
    static ScreenManager mgr;
    return &mgr;
}

void ScreenManager::loadScreenByScreenEnum(SCREEN_TYPE_E screen_id)
{
    _current_screen._screen_handle = lv_scr_act();
    while(_screen_stack.size() > 1) {
        invokeCloseScreenCallback(_current_screen._screen_id);
        
        _current_screen = _screen_stack.back();
        _screen_stack.pop_back();
    }

    loadScreen(_current_screen._screen_handle, screen_id);
}

void ScreenManager::loadScreenById(const char* widget_id)
{
    SCREEN_TYPE_E screen_id = screenIdToEnum(widget_id);
    loadScreenByScreenEnum(screen_id);
}

void ScreenManager::updateObjectTree()
{
    lv_obj_tree_walk(_current_screen._screen_handle, objTreeWalkCb, &_current_screen._children);
}

std::vector<lv_obj_t *> ScreenManager::findObjectsByLabelText(const char* needle) const
{
    
    std::vector<lv_obj_t *> res;
    for(const auto &[parent, children] : _current_screen._children){
        if(parent && isObjectWithMatchingText(parent, needle)){
            res.push_back(parent);
        }
        for(const auto &child: children){
            if(isObjectWithMatchingText(child, needle)){
                res.push_back(child);
            }
        }
    }
    return res;
}

std::vector<lv_obj_t *> ScreenManager::findObjectsAtPosition(int32_t x, int32_t y) const
{
    std::vector<lv_obj_t *> res;
    for(const auto &[parent, children] : _current_screen._children){
        if(parent && 
            lv_obj_is_valid(parent) && 
            isPositionInsideRect(x, y, getObjectRect(parent))){
            res.push_back(parent);
        }
        for(const auto &obj: children){
            if(obj &&
                lv_obj_is_valid(obj) &&
                isPositionInsideRect(x, y, getObjectRect(obj))){
                res.push_back(obj);
            }

            // if(lv_obj_is_valid(obj) && lv_obj_get_class(obj) == &lv_label_class){//} && 
            //     // isObjectWithMatchingText(obj, "Connected")){
            //     const char * txt = lv_label_get_text(obj);
            //     if(txt && *txt && strstr(txt, "Status : Connected")){
            //         if(strcmp(txt, "Status : Connected") == 0){
            //             int a = 0;
            //             a = 1;
            //         }
            //         auto rect = getObjectRect(obj);
            //         rect = getObjectRect(obj);
            //         bool ret = isPositionInsideRect(x, y, rect);
            //         int a = 0;
            //         a = 1;
            //     }
            // }
        }
    }
    return res;
}

void ScreenManager::loadScreen(lv_obj_t *previous_screen, SCREEN_TYPE_E screen_id)
{
    if(_current_screen._screen_id == screen_id){
        LOG(WARNING, ScreenManager, "Not loading screen because it is already loaded, screen_id:%i\n", screen_id);
        return;
    }
    LOG(DEBUG, ScreenManager, "Loading screen %s\n", screenEnumToString(screen_id));
    _screen_stack.push_back(ScreenContext(_current_screen._screen_id, previous_screen));
    _current_screen._screen_id = screen_id;
    _current_screen._screen_handle = createScreen(screen_id);
    if(!_current_screen._screen_handle){
        LOG(FATAL, ScreenManager, "Failed loading screen, screen_id:%i\n", screen_id);
        return;
    }
    lv_scr_load(_current_screen._screen_handle);
    // lv_obj_t *parent = _current_screen._screen_handle;
    updateObjectTree();

    _screen_stack.back()._children = _current_screen._children;

    //osm todo
    // ScreenLoaded msg;
    // strcpy(msg._screen_id, screenEnumToString(screen_id));
    // ipc_send_screen_loaded_message(&msg);
}

SCREEN_TYPE_E ScreenManager::getPreviousScreenId()
{
    return _screen_stack.back()._screen_id;
}
SCREEN_TYPE_E ScreenManager::getCurrentScreenId()
{
    return _current_screen._screen_id;
}

void ScreenManager::unLoadScreen()
{
    invokeCloseScreenCallback(_current_screen._screen_id);
    if(_current_screen._screen_handle){
        lv_obj_del(_current_screen._screen_handle);
    }
    _current_screen = _screen_stack.back();
    _screen_stack.pop_back();
    if(_current_screen._screen_handle){
        lv_scr_load(_current_screen._screen_handle);
    }
}

void ScreenManager::clearScreenStack()
{
    do{
        if(_current_screen._screen_id != NO_SCREEN){
            invokeCloseScreenCallback(_current_screen._screen_id);
            LOG(DEBUG, ScreenManager, "Screen id = %i\n", _current_screen._screen_id);
        }
        if(_current_screen._screen_handle){
            lv_obj_del(_current_screen._screen_handle);
        }
        if(!_screen_stack.empty()){
            _current_screen = _screen_stack.back();
            _screen_stack.pop_back();
            LOG(DEBUG, ScreenManager, "Popped screen = %i\n", _current_screen._screen_id);
        }
    }while(!_screen_stack.empty());
}


void ScreenManager::invokeCloseScreenCallback(SCREEN_TYPE_E screen_id)
{
    try{
        CloseScreenCallback callback = _on_close_screen_callbacks.at(screen_id);
        callback(nullptr);
    }
    catch(std::out_of_range &e){
        LOG(WARNING, ScreenManager, "Callback for screen id %i not registered", _current_screen._screen_id);
    }
}

void ScreenManager::registerCloseScreenCallback(const char *screen_id, CloseScreenCallback callback)
{
    SCREEN_TYPE_E iscreen_id = screenIdToEnum(screen_id);
    auto search = _on_close_screen_callbacks.find(iscreen_id);
    if(search == _on_close_screen_callbacks.end()){
        _on_close_screen_callbacks[iscreen_id] = callback;
    }
}

std::optional<lv_area_t> ScreenManager::getObjectRect(const lv_obj_t *obj, int depth) const
{
    if(!obj){
        return std::nullopt;
    }
    lv_area_t area;
    lv_obj_get_coords(obj, &area);
    // area.x1 = lv_obj_get_x(obj);
    // area.x2 = lv_obj_get_x2(obj);
    // area.y1 = lv_obj_get_y(obj);
    // area.y2 = lv_obj_get_y2(obj);
    return area;

    // std::optional<lv_area_t> o_area = std::nullopt;
    // std::optional<lv_area_t> o_parent = std::nullopt;
    // if(obj->parent){
    //     o_parent = getObjectRect(obj->parent, depth + 1);
    // }
    // lv_style_value_t value = {0};
    // lv_res_t res;
    // lv_area_t area;
    
    // res = lv_obj_get_local_style_prop(const_cast<lv_obj_t *>(obj), LV_STYLE_X, &value, 0);
    // area.x1 = (res == LV_RES_OK) ? value.num : obj->coords.x1;

    // res = lv_obj_get_local_style_prop(const_cast<lv_obj_t *>(obj), LV_STYLE_Y, &value, 0);
    // area.y1 = (res == LV_RES_OK) ? value.num : obj->coords.y1;
    
    // res = lv_obj_get_local_style_prop(const_cast<lv_obj_t *>(obj), LV_STYLE_WIDTH, &value, 0);
    // area.x2 = (res == LV_RES_OK) ? area.x1 + value.num : obj->coords.x2;

    // res = lv_obj_get_local_style_prop(const_cast<lv_obj_t *>(obj), LV_STYLE_HEIGHT, &value, 0);
    // area.y2 = (res == LV_RES_OK) ? area.y1 + value.num : obj->coords.y2;

    // if(o_parent){
    //     area.x1 += o_parent->x1;
    //     area.y1 += o_parent->y1;
    //     //area.x2 += o_parent->x2;
    //     //area.y2 += o_parent->y2;
    // }
    // o_area = area;
    // return o_area;
}

