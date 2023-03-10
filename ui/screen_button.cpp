#include "screen_button.h"

#include <cJSON.h>
#include <file_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
// #include <rendering_engine.h>
// #include <sensorboard_reader.h>
// #include <functionkey_reader.h>
#include <json_utils.h>
// #include <mq_utils.h>
#include <debug_logger.h>
#include <time.h>
#include <language_translation.h>

// #include "audio_setup.h"
#include "json_gui_loader.h"
// #include "main_setup_time.h"
// #include "keyboard_adjust.h"
// #include "keyboard_profile.h"
// #include "keyboard_hardware_type_screen.h"
// #include "wifi_options_settings.h"
// #include "main_setup_display_color.h"
// #include "main_setup_display_font.h"
// #include "main_setup_backlight.h"
// #include "main_setup_options.h"
// #include "main_setup_translation.h"
// #include "main_setup_dictionary.h"
// #include "main_setup_display_color.h"
// #include "reopen_jobs.h"
// #include "storage_menu.h"
// #include "test_mode.h"
// #include "writers_menu.h"
// #include "keyboardlayout.h"
// #include "wifi_search.h"
// #include "wifi_connect.h"
// #include "wifi_options_manager.h"
// #include "bluetooth_wifi_information.h"
// #include "language_translation.h"
// #include "main_menu_setup2.h"
// #include "Full_Screen_Image.h"
// #include <writers_menu.h>

LV_FONT_DECLARE(title_bar_icons);

#define MAGNIFIER_SYMBOL "\xEF\x80\x82"
#define GEAR_SYMBOL "\xEF\x80\x93"

LOG_CATEGORY(BTN, "BTN")

static cJSON *speed_record_json = NULL;
static char data_json_file[256];

void on_button_press_navigate_to_screen(const char *json)
{
  LOG(DEBUG, BTN, "on_button_press_navigate_to_screen json: '%s'\n", json);

  if (!json || !*json)
  {
    return;
  }

  cJSON *json_obj = cJSON_Parse(json);
  if (json_obj == NULL)
  {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL)
    {
      LOG(WARNING, BTN, "Error before: '%s'\n", error_ptr);
    }
    return;
  }
  cJSON *screen = cJSON_GetObjectItemCaseSensitive(json_obj, "args");
  LOG(DEBUG, BTN, "screen_id %s\n", screen->valuestring);
  loadScreenById(screen->valuestring);
  cJSON_Delete(json_obj);
}

static void 
on_press_switchfoo(const char* json) {
  (void)json;
}

static void 
on_press_checkboxfoo(const char *json)
{
  (void)json;
}

static void 
update_sensorboard_keystroke_bars(lv_obj_t *chart)
{
  //osm todo
  // update_chart_from_keystroke_data(
  //     chart,
  //     reinterpret_cast<int16_t*>(scan_alias.usReading),
  //     sizeof(tmp_sample._data) / sizeof(tmp_sample._data[0]));
}

static void 
update_functionkey_keystroke_bars(lv_obj_t *chart)
{
}

struct ButtonEventHandler
{
  const char _id[128];
  char *_callback_json_arg;
  void (*_event_cb)(const char* json);
};

static struct ButtonEventHandler _button_event_handlers[] = {
  {"page/steno-strokes/date", NULL, on_button_press_navigate_to_screen},
  {"page/steno-strokes/accept", NULL, NULL },
  {"page/steno-strokes/quit", NULL, NULL },
  {"page/date-time/next", NULL, NULL },
  {"page/date-time/blank", NULL, on_button_press_navigate_to_screen},
  {"page/date-time/accept", NULL, NULL },
  {"page/date-time/quit", NULL, NULL },
  {"page1/functionkey", NULL, on_button_press_navigate_to_screen },
  {"page1/sensorboardkey", NULL, on_button_press_navigate_to_screen },
  {"\0", NULL, NULL},
};

struct ButtonSymbol
{
  char _button_id[128];
  const char *_symbol;
};

static struct ButtonSymbol _button_symbols[] ={
  {"writers_menu1/lastq", MAGNIFIER_SYMBOL},
  {"writers_menu2/search", MAGNIFIER_SYMBOL},
  {"writers_menu2/setup", GEAR_SYMBOL},
  {"\0", NULL},
};

static void add_button_symbols(lv_obj_t *label, const char *button_id, const char *label_text){
  bool symbolAvailable = false;
  for(int i=0; _button_symbols[i]._button_id[0] !=0; i++){
    if(strcmp(_button_symbols[i]._button_id, button_id) == 0){
      lv_label_set_text(label, "");
      lv_obj_t *symbol = lv_label_create(label);
      lv_obj_set_style_text_font(symbol, &title_bar_icons, 0);
      lv_label_set_text(symbol, _button_symbols[i]._symbol);

      lv_obj_t *text = lv_label_create(label);
      lv_label_set_text(text, tr(label_text));
      lv_obj_align_to(text, symbol, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
      symbolAvailable = true;
    }
  }
  if(!symbolAvailable){
    lv_label_set_text(label, tr(label_text));
  }
}

static bool file_exists (const char *filename) 
{
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}


static void chart_update_timer(struct _lv_timer_t *timer)
{
  lv_obj_t *chart = (lv_obj_t *)timer->user_data;
  struct ChartContext *ctx = (struct ChartContext *)chart->user_data;
  //  LOG(DEBUG, BTN, "called in timer (%p) with chart (%p), with chart data (%p)\n", timer, timer->user_data, chart->user_data);
  if (ctx->_update_timer_function && *ctx->_update_timer_function)
  {
    if (strcmp(ctx->_update_timer_function, "update_sensorboard_keystroke_bars") == 0)
    { // osm todo use an array to track these are valid, or register in beginning
      update_sensorboard_keystroke_bars(chart);
    }
    else if (strcmp(ctx->_update_timer_function, "update_functionkey_keystroke_bars") == 0)
    {
      update_functionkey_keystroke_bars(chart);
    }
  }
}
static void event_cb(lv_event_t *e)
{
  size_t btn_idx = (size_t)e->user_data;
  LOG(DEBUG, BTN, "btn event, btn_idx %li, id '%s', arg '%s'\n",
      btn_idx,
      _button_event_handlers[btn_idx]._id,
      _button_event_handlers[btn_idx]._callback_json_arg ? _button_event_handlers[btn_idx]._callback_json_arg : "");
  if (_button_event_handlers[btn_idx]._event_cb)
  {
    _button_event_handlers[btn_idx]._event_cb(
        _button_event_handlers[btn_idx]._callback_json_arg);
  }
}
//  lv_event_cb_t event_cb,

static bool register_button_callback(lv_obj_t *btn, const char* id, const char* on_press){
  for(size_t btn_id = 0; _button_event_handlers[btn_id]._id[0] != 0; btn_id++) {
    if(strcmp(id , _button_event_handlers[btn_id]._id) == 0) {
        if(on_press && *on_press) {
          _button_event_handlers[btn_id]._callback_json_arg = (char*)realloc(
            _button_event_handlers[btn_id]._callback_json_arg, strlen(on_press) + 1);
        strcpy(_button_event_handlers[btn_id]._callback_json_arg, on_press);
      }
      else if (_button_event_handlers[btn_id]._callback_json_arg)
      {
        free(_button_event_handlers[btn_id]._callback_json_arg);
        _button_event_handlers[btn_id]._callback_json_arg = NULL;
      }
      lv_obj_add_event_cb(btn, event_cb, LV_EVENT_RELEASED, (void *)btn_id);
      return true;
    }
  }
  return false;
}

static void delete_button_userdata(lv_event_t *e)
{
  ButtonData_T *buttonData = (ButtonData_T *)e->user_data;
  if (buttonData)
  {
    if (buttonData->_style)
    {
      lv_style_reset(buttonData->_style);
      free(buttonData->_style);
    }
    if (buttonData->_id)
    {
      free(buttonData->_id);
    }
    free(buttonData);
  }
}

lv_obj_t *create_button(
    lv_obj_t *parent,
    const char *id,
    int posx,
    int posy,
    int width,
    int height,
    const char *label_text,
    int background_color,
    int border_color,
    int gradient_color,
    const char *on_press)
{
  if (!id){
    LOG(WARNING, BTN, "Need an id for the widget\n");
    return NULL;
  }

  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_pos(btn, posx, posy);
  lv_obj_set_size(btn, width, height);

  if (!register_button_callback(btn, id, on_press)){
    LOG(FATAL, BTN, "No callback found in C code for '%s' in 'static struct ButtonEventHandler _button_event_handlers[]'\n", id);
  }

  lv_obj_t *label;
  label = lv_label_create(btn);
  printf("label text = %s\n", label_text);
  add_button_symbols(label, id, label_text);
  // lv_label_set_text(label, tr(label_text));
  lv_obj_center(label);
  lv_color_t bd_color = { .full = (uint32_t)border_color };
  lv_color_t bg_color = { .full = (uint32_t)background_color };
  lv_color_t gr_color = { .full = (uint32_t)gradient_color }; 
  lv_style_t *style = (lv_style_t *) malloc(sizeof(lv_style_t));
  ButtonData_T *button_data = (ButtonData_T*)malloc(sizeof(ButtonData_T));
  button_data->_style = (lv_style_t *) malloc(sizeof(lv_style_t));
  button_data->_id = strdup(id);
  lv_style_init(button_data->_style);
  lv_style_set_radius(button_data->_style, 0);

  lv_style_set_border_color(button_data->_style, bd_color);
  lv_style_set_border_width(button_data->_style, 2);
  lv_style_set_border_opa(button_data->_style, LV_OPA_50);
  lv_style_set_border_side(button_data->_style, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT);

  lv_style_set_bg_color(button_data->_style, bg_color);
  lv_style_set_bg_opa(button_data->_style, LV_OPA_80);

  if (gradient_color != 0){

    lv_style_set_bg_opa(button_data->_style, LV_OPA_COVER);
    lv_style_set_bg_color(button_data->_style, bg_color);
    lv_style_set_bg_grad_color(button_data->_style, gr_color);
    lv_style_set_bg_grad_dir(button_data->_style, LV_GRAD_DIR_HOR);
    lv_style_set_bg_main_stop(button_data->_style, 255 - (width < 100 ? 10 : 5));
    lv_style_set_bg_grad_stop(button_data->_style, 255);
  }

  lv_obj_add_style(btn, button_data->_style, LV_PART_MAIN);
  lv_obj_set_user_data(btn, button_data);
  lv_obj_add_event_cb(btn, delete_button_userdata, LV_EVENT_DELETE, (void *)button_data);

  return btn;
}

lv_obj_t *create_switch(
    lv_obj_t *parent,
    const char *id,
    int posx,
    int posy,
    const char *label_text,
    const char *on_press)
{
  if (!id){
    LOG(WARNING, BTN, "Need an id for the widget\n");
    return NULL;
  }
  // lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
  // lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  lv_obj_t *sw = lv_switch_create(parent);
  lv_obj_set_pos(sw, posx, posy);
  lv_obj_add_state(sw, LV_STATE_CHECKED);

  register_button_callback(sw, id, on_press);

  return sw;
}

lv_obj_t *create_checkbox(
    lv_obj_t *parent,
    const char *id,
    int posx,
    int posy,
    const char *label_text,
    const char *on_press)
{
  if (!id)
  {
    LOG(WARNING, BTN, "Need an id for the widget\n");
    return NULL;
  }

  lv_obj_t *cb = lv_checkbox_create(parent);
  lv_obj_set_pos(cb, posx, posy);
  lv_checkbox_set_text(cb, label_text);

  register_button_callback(cb, id, on_press);
  return cb;
}

static int get_array_size(const char **array)
{
  int i;
  for (i = 0; array && array[i]; ++i)
    ;
  return i;
}

static void init_chart_series_context(
    struct ChartSeriesContext *ctx,
    lv_obj_t *chart,
    int num_xlabels,
    char *series_name)
{
  ctx->_series_name = series_name;
  ctx->_lv_series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
  ctx->_num_points = num_xlabels;
  ctx->_data_sz = (num_xlabels + 1) * sizeof(lv_coord_t);
  ctx->_data = (lv_coord_t *)malloc(ctx->_data_sz);
  memset(ctx->_data, 0, ctx->_data_sz);
}
static void on_draw_chart(lv_event_t *e)
{
  lv_obj_t *chart = (lv_obj_t *)e->user_data;
  lv_obj_draw_part_dsc_t * dsc = (lv_obj_draw_part_dsc_t*)lv_event_get_param(e);
  if(dsc->part == LV_PART_TICKS && dsc->id == LV_CHART_AXIS_PRIMARY_X) {
    const struct ChartContext *ctx = (const struct ChartContext *)chart->user_data;
    lv_snprintf(dsc->text, sizeof(dsc->text), "%s", ctx->_xlabels[dsc->value]);
  }
}

lv_obj_t *create_chart(
    lv_obj_t *parent,
    const char *id,
    int posx,
    int posy,
    int width,
    int height,
    const char *type,
    char **xlabels,
    char **ylabels,
    int ymin,
    int ymax,
    char **series_names,
    char *update_timer_function,
    int update_hz)
{
  if (!type || !*type || strcmp(type, "bar") != 0)
  {
    LOG(FATAL, BTN, "Unknown chart type specified: '%s'\n",
        type && *type ? type : "");
  }

  lv_obj_t *chart = lv_chart_create(parent);
  lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
  int num_xlabels = get_array_size((const char **)xlabels);
  int num_ylabels = get_array_size((const char **)ylabels);
  lv_chart_set_point_count(chart, num_xlabels);

  lv_obj_set_size(chart, MONITOR_HOR_RES, MONITOR_VER_RES - 100); // osm todo, use width, height but it has % in it
  lv_obj_align(chart, LV_ALIGN_TOP_LEFT, posx, posy);

  lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, num_ylabels, 5, num_ylabels, 5, true, 40);
  lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, num_xlabels, 5, num_xlabels, 1, true, 0);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, ymin, ymax);

  lv_obj_add_event_cb(chart, on_draw_chart, LV_EVENT_DRAW_PART_BEGIN, chart);
  chart->user_data = xlabels;
  lv_obj_refresh_ext_draw_size(chart);

  // cursor = lv_chart_add_cursor(_chart, lv_palette_main(LV_PALETTE_BLUE), LV_DIR_LEFT | LV_DIR_BOTTOM);

  int num_series = get_array_size((const char **)series_names);
  struct ChartSeriesContext *series_contexts = (struct ChartSeriesContext *)malloc(sizeof(struct ChartSeriesContext) * num_series);
  int cnt;
  for (cnt = 0; cnt < num_series; ++cnt)
  {
    init_chart_series_context(
        &series_contexts[cnt],
        chart,
        num_xlabels,
        series_names[cnt]);
    lv_chart_set_ext_y_array(chart, series_contexts[cnt]._lv_series, series_contexts[cnt]._data);
  }
  struct ChartContext *ctx = (struct ChartContext *)malloc(sizeof(struct ChartContext));
  ctx->_id = strdup(id);
  ctx->_xlabels = xlabels;
  ctx->_ylabels = ylabels;
  ctx->_num_series = num_series;
  ctx->_series = series_contexts;
  if (update_timer_function && *update_timer_function)
  {
    ctx->_update_timer_function = strdup(update_timer_function);
    ctx->_update_timer = lv_timer_create(chart_update_timer, 1000 / update_hz, chart);
  }
  chart->user_data = ctx;

  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, "Keyboard Hardware Type: Standard");
  lv_obj_align_to(label, chart, LV_ALIGN_OUT_TOP_MID, 0, -5);

  // static size_t btn_id[8] = {0,1,2,3,4,5,6,7};
  // static int btn_count = sizeof(btn_id)/sizeof(btn_id[0]);
  // static char btn_labels[8][32] = {"More", "Less", " ", "Date",  "Time", " ", "Reset", "Done"};
  // for(int i = 0; i < btn_count; ++i) {
  //     lv_obj_t* btn = create_button(
  //         _screen,
  //         0 + i*MONITOR_HOR_RES/btn_count,
  //         MONITOR_VER_RES - BUTTON_HEIGHT,
  //         MONITOR_HOR_RES/btn_count,
  //         BUTTON_HEIGHT,
  //         btn_labels[i],
  //         btn_event_cb,
  //         (void*)btn_id[i]);
  // }
  // _on_close = on_close;

  return chart;
}

lv_obj_t** create_grid(
  lv_obj_t *screen,
  int y_offset,
  int width,
  int height,
  int row_height,
  uint32_t grid_color,
  int total_num_options){
  lv_obj_t **grid_labels = (lv_obj_t **)malloc(total_num_options * sizeof(lv_obj_t *));
  static lv_style_t transparent_style;
  lv_style_init(&transparent_style);
  lv_style_set_bg_opa(&transparent_style, LV_OPA_0);
  lv_style_set_border_width(&transparent_style, 0);

  static lv_style_t bg_style;
  lv_style_init(&bg_style);
  lv_style_set_bg_color(&bg_style, lv_color_hex(grid_color));
  lv_style_set_border_width(&bg_style, 0);

  static lv_style_t grid_style;
  lv_style_init(&grid_style);
  lv_style_set_border_width(&grid_style, 0);

  lv_obj_t *grid_list = lv_list_create(screen);
  lv_obj_set_pos(grid_list, 0, y_offset);
  lv_obj_set_size(grid_list, width, height);

  lv_obj_add_style(grid_list, &grid_style, LV_PART_MAIN);
  
  // Add list content
  for (int i = 0; i < total_num_options; i++)
  {
    grid_labels[i] = lv_list_add_text(grid_list, "");
    lv_obj_set_height(grid_labels[i], row_height);
    if (i % 2)
    {
      lv_obj_add_style(grid_labels[i], &transparent_style, LV_PART_MAIN);
    }
    else
    {
      lv_obj_add_style(grid_labels[i], &bg_style, LV_PART_MAIN);
    }
  }
  return grid_labels;
}

lv_obj_t** create_grid_with_scroll(
  lv_obj_t *screen,
  int y_offset,
  int width,
  int height,
  int row_height,
  uint32_t grid_color,
  int total_num_options){
  lv_obj_t **grid_labels = (lv_obj_t **)malloc(total_num_options * sizeof(lv_obj_t *));
  static lv_style_t transparent_style;
  lv_style_init(&transparent_style);
  lv_style_set_bg_opa(&transparent_style, LV_OPA_0);
  lv_style_set_border_width(&transparent_style, 0);

  static lv_style_t bg_style;
  lv_style_init(&bg_style);
  lv_style_set_bg_color(&bg_style, lv_color_hex(grid_color));
  lv_style_set_border_width(&bg_style, 0);

  static lv_style_t grid_style;
  lv_style_init(&grid_style);
  lv_style_set_border_width(&grid_style, 0);

  // Clear parent screen scrolling so that scrolling is applicable only to grid
  
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  // lv_list doesn't have scrolling itself, it need an container to hold and does scrolling
  // Create a container to get scrolling to list
  lv_obj_t *page = lv_obj_create(screen);
  lv_obj_set_pos(page, 0, y_offset);
  lv_obj_set_size(page, width, height);
  
  // Create a list view and align with container i.e. page
  lv_obj_t *grid_list = lv_list_create(page);
  lv_obj_align(grid_list, LV_ALIGN_TOP_MID, 0, -15);
  lv_obj_set_size(grid_list, width-50, height-140);
  lv_obj_add_style(grid_list, &grid_style, LV_PART_MAIN);
  
  // Add list content
  for (int i = 0; i < total_num_options; i++)
  {
    grid_labels[i] = lv_list_add_text(grid_list, "");
    lv_obj_set_height(grid_labels[i], row_height);
    if (i % 2)
    {
      lv_obj_add_style(grid_labels[i], &transparent_style, LV_PART_MAIN);
    }
    else
    {
      lv_obj_add_style(grid_labels[i], &bg_style, LV_PART_MAIN);
    }
  }
  return grid_labels;
}


int dropdown_get_initial_index(char *options, const char *initial_value)
{
  int index = 0;
  bool found = false;
  char *token = strtok(options, "\n");
  while (token != NULL)
  {
    if (strcmp(token, initial_value) == 0)
    {
      found = true;
      break;
    }
    else
    {
      index++;
      token = strtok(NULL, "\n");
    }
  }
  if (!found)
  {
    index = 0; // select 1st entry by default
  }
  return index;
}

lv_obj_t *create_drop_down(
    lv_obj_t *parent,
    int x_offset,
    char *options,
    int width,
    const char *initial_value)
{
  lv_obj_t *dd = lv_dropdown_create(parent);
  lv_obj_set_width(dd, width);
  lv_obj_align_to(dd, parent, LV_ALIGN_CENTER, x_offset, 0);
  lv_dropdown_set_options(dd, options);
  lv_obj_set_user_data(parent, dd);
  int index = dropdown_get_initial_index(options, initial_value);
  lv_dropdown_set_selected(dd, index);
  return dd;
}

lv_obj_t *create_drop_down1(
    lv_obj_t *parent,
    int x_offset,
    char *options,
    int width,
    char *initial_value,
    void (*callback)(lv_event_t*))
{
  lv_obj_t *dd = lv_dropdown_create(parent);
  lv_obj_set_width(dd, width);
  lv_obj_align_to(dd, parent, LV_ALIGN_CENTER, x_offset, 0);
  lv_dropdown_set_options(dd, options);
  lv_obj_set_user_data(parent, dd);
  int index = dropdown_get_initial_index(options, initial_value);
  lv_obj_add_event_cb(dd, callback, LV_EVENT_ALL, (void*)dd);
  lv_dropdown_set_selected(dd, index);
  return dd;
}

static void delete_switch_userdata(lv_event_t *e)
{
  SwitchWithLabel_T *obj = (SwitchWithLabel_T *)e->user_data;
  if (obj)
  {
    if (obj->_style_main)
    {
      lv_style_reset(obj->_style_main);
      free(obj->_style_main);
    }
    if (obj->_style_indicator)
    {
      lv_style_reset(obj->_style_indicator);
      free(obj->_style_indicator);
    }
    if (obj->_style_knob)
    {
      lv_style_reset(obj->_style_knob);
      free(obj->_style_knob);
    }
    free(obj);
  }
}

SwitchWithLabel_T *create_switch_with_label(
    lv_obj_t *parent,
    int width,
    int height,
    int x_offset,
    int option_type,
    bool is_checked,
    int checked_color,
    // lv_style_t *text_style,
    void (*callback)(lv_event_t*)
){
    static const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, LV_STYLE_PROP_INV};
    static lv_style_transition_dsc_t transition_dsc;
    lv_style_transition_dsc_init(&transition_dsc, props, lv_anim_path_linear, 300, 0, NULL);

    SwitchWithLabel_T *switch_type = (SwitchWithLabel_T *)malloc(sizeof(SwitchWithLabel_T));
    switch_type->_style_main = (lv_style_t *) malloc(sizeof(lv_style_t));
    switch_type->_style_indicator = (lv_style_t *) malloc(sizeof(lv_style_t));
    switch_type->_style_knob = (lv_style_t *) malloc(sizeof(lv_style_t));

    switch_type->_option_type = option_type;
    switch_type->_sw = lv_switch_create(parent);

    lv_style_init(switch_type->_style_main);
    lv_style_set_bg_opa(switch_type->_style_main, LV_OPA_70);
    lv_style_set_bg_color(switch_type->_style_main, lv_color_hex(checked_color));
    lv_style_set_bg_color(switch_type->_style_main, lv_color_hex3(0xbbb));
    lv_style_set_radius(switch_type->_style_main, LV_RADIUS_CIRCLE);
    lv_style_set_pad_ver(switch_type->_style_main, 0); /*Makes the indicator larger*/

    lv_style_init(switch_type->_style_indicator);
    lv_style_set_bg_opa(switch_type->_style_indicator, LV_OPA_70);
    lv_style_set_bg_color(switch_type->_style_indicator, lv_color_hex(checked_color));
    lv_style_set_radius(switch_type->_style_indicator, LV_RADIUS_CIRCLE);
    lv_style_set_transition(switch_type->_style_indicator, &transition_dsc);

    lv_style_init(switch_type->_style_knob);
    lv_style_set_bg_opa(switch_type->_style_knob, LV_OPA_70);
    lv_style_set_bg_color(switch_type->_style_knob, lv_color_hex(checked_color));
    lv_style_set_border_color(switch_type->_style_knob, lv_color_hex(checked_color));
    lv_style_set_border_width(switch_type->_style_knob, 0);
    lv_style_set_radius(switch_type->_style_knob, LV_RADIUS_CIRCLE);
    lv_style_set_pad_all(switch_type->_style_knob, 3); /*Makes the knob larger*/
    lv_style_set_transition(switch_type->_style_knob, &transition_dsc);


    lv_obj_align_to(switch_type->_sw, parent, LV_ALIGN_CENTER, x_offset, 0);
    lv_obj_set_size(switch_type->_sw, width, height);
    lv_obj_add_style(switch_type->_sw, switch_type->_style_main, LV_PART_MAIN);
    lv_obj_add_style(switch_type->_sw, switch_type->_style_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(switch_type->_sw, switch_type->_style_indicator, LV_PART_INDICATOR | LV_STATE_CHECKED );
    lv_obj_add_style(switch_type->_sw, switch_type->_style_knob, LV_PART_KNOB);
  
    switch_type->_label = lv_label_create(parent);
    // lv_obj_add_style(switch_type->_label, text_style, LV_PART_MAIN);
    lv_obj_set_size(switch_type->_label, 40, 40);
    lv_obj_align_to(switch_type->_label, switch_type->_sw, LV_ALIGN_OUT_LEFT_MID, 10, 10);
    lv_obj_add_event_cb(switch_type->_sw, callback, LV_EVENT_ALL, (void*)switch_type);
    lv_obj_set_user_data(parent, switch_type);
    if(is_checked){
        lv_obj_add_state(switch_type->_sw, LV_STATE_CHECKED);
    }
    lv_obj_add_event_cb(switch_type->_sw, delete_switch_userdata, LV_EVENT_DELETE, (void*)switch_type);
    return switch_type;
}

static void delete_slider_data(lv_event_t *e)
{
  Slider_T *s = (Slider_T *)e->user_data;
  if (s)
  {
    if (s->_style_indicator)
    {
      lv_style_reset(s->_style_indicator);
      free(s->_style_indicator);
    }
    free(s);
  }
}

Slider_T *create_slider(
    lv_obj_t *parent,
    int x_offset,
    int label_length,
    int slider_width,
    int min_value,
    int max_value,
    int initial_value,
    int slider_color,
    const char *label_text,
    void (*slider_movement_callback)(lv_event_t *))
{
  Slider_T *s = (Slider_T *)malloc(sizeof(Slider_T));
  s->_style_indicator = (lv_style_t *)malloc(sizeof(lv_style_t));
  s->_label = lv_label_create(parent);
  lv_obj_align_to(s->_label, parent, LV_ALIGN_CENTER, x_offset, 0);
  lv_obj_set_size(s->_label, label_length, label_length);
  lv_label_set_text(s->_label, label_text);

  // static lv_style_t style_indicator;
  lv_style_init(s->_style_indicator);
  lv_style_set_bg_opa(s->_style_indicator, LV_OPA_70);
  lv_style_set_bg_color(s->_style_indicator, lv_color_hex(slider_color));

  s->_slider = lv_slider_create(parent);
  lv_obj_align_to(s->_slider, s->_label, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);
  lv_obj_set_width(s->_slider, slider_width);
  lv_obj_add_event_cb(s->_slider, slider_movement_callback, LV_EVENT_ALL, s);
  lv_slider_set_range(s->_slider, min_value, max_value);
  lv_slider_set_value(s->_slider, initial_value, LV_ANIM_OFF);
  lv_obj_add_style(s->_slider, s->_style_indicator, LV_PART_INDICATOR);
  lv_obj_add_style(s->_slider, s->_style_indicator, LV_PART_KNOB);
  lv_obj_set_user_data(parent, s);
  lv_obj_add_event_cb(s->_slider, delete_slider_data, LV_EVENT_DELETE, (void *)s);
  return s;
}

lv_obj_t * update_selection_pointer(
    lv_obj_t *current_pointer,
    lv_obj_t *new_selection,
    int x_offset,
    lv_style_t *style)
{
  if (current_pointer){
    
    lv_obj_del(current_pointer);
    current_pointer = NULL;
  }
  current_pointer = lv_label_create(new_selection);
  lv_obj_set_size(current_pointer, 50, 50);
  lv_obj_align_to(current_pointer, new_selection, LV_ALIGN_CENTER, -x_offset, 20);
  lv_label_set_text(current_pointer, ">>");
  lv_obj_add_style(current_pointer, style, LV_PART_MAIN);
  return current_pointer;
}

lv_obj_t * update_selection_pointer_color(
    lv_obj_t *current_pointer,
    lv_obj_t *new_selection,
    int x_offset,
    lv_style_t *style)
{
  if (current_pointer){
    //lv_obj_clean(current_pointer);
    // lv_label_set_text(current_pointer, "");
    lv_obj_del(current_pointer);
    current_pointer = NULL;
  }
  current_pointer = lv_label_create(new_selection);
  lv_obj_set_size(current_pointer, 50, 50);
  lv_obj_align_to(current_pointer, new_selection, LV_ALIGN_LEFT_MID, -2, 17);
  lv_label_set_text(current_pointer, ">>");
  lv_style_set_text_color(style,lv_color_white());
  lv_obj_add_style(current_pointer, style, LV_PART_MAIN);
  return current_pointer;
}

lv_obj_t *steno_popup(
    lv_obj_t *parent,
    int width,
    int height,
    int header_height,
    const char *title,
    const char *content)
{
  static lv_style_t header_style;
  lv_style_init(&header_style);
  lv_style_set_bg_color(&header_style, lv_palette_main(LV_PALETTE_INDIGO));
  lv_style_set_text_color(&header_style, lv_color_hex(0xffffff));

  static lv_style_t content_style;
  lv_style_init(&content_style);
  lv_style_set_bg_color(&content_style, lv_color_lighten(lv_palette_main(LV_PALETTE_INDIGO), 200));

  lv_obj_t *pop_up = lv_win_create(parent, header_height);
  lv_obj_set_size(pop_up, width, height);
  lv_win_add_title(pop_up, title);

  lv_obj_t *header = lv_win_get_header(pop_up);
  lv_obj_add_style(header, &header_style, LV_PART_MAIN);

  lv_obj_t *cont = lv_win_get_content(pop_up);
  lv_obj_t *label = lv_label_create(cont);
  lv_obj_add_style(cont, &content_style, LV_PART_MAIN);
  lv_label_set_text(label, content);
  return pop_up;
}

