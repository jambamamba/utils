#pragma once

#include <lvgl/lvgl.h>
#include <cJSON_Utils.h>

typedef struct ButtonData
{
  lv_style_t* _style;
  char* _id;
}ButtonData_T; 

typedef struct SwitchWithLabel{
    lv_obj_t *_sw;
    lv_obj_t *_label;
    int _option_type;
    lv_style_t* _style_main;
    lv_style_t* _style_indicator;
    lv_style_t* _style_knob;
}SwitchWithLabel_T;

typedef struct slider_struct{
    lv_obj_t *_label;
    lv_obj_t *_slider;
    lv_style_t* _style_indicator;
    
}Slider_T;

lv_obj_t *create_button(
  lv_obj_t *parent,
  const char* id,
  int posx, 
  int posy, 
  int width, 
  int height, 
  const char *label_text,
  int background_color, 
  int border_color,
  int gradient_color, 
  const char *on_press);

lv_obj_t *create_switch(
  lv_obj_t *parent,
  const char* id,
  int posx, 
  int posy, 
  const char *label_text,
  const char *on_press);

lv_obj_t *create_checkbox(
  lv_obj_t *parent,
  const char* id,
  int posx, 
  int posy, 
  const char *label_text,
  const char *on_press);


struct ChartSeriesContext {
  char *_series_name;
  lv_chart_series_t *_lv_series;
  size_t _num_points;
  size_t _data_sz; 
  lv_coord_t *_data; 
};
struct ChartContext {
  char *_id;
  char **_xlabels;
  char **_ylabels;
  char *_update_timer_function;
  int _num_series;
  struct ChartSeriesContext *_series;
  lv_timer_t *_update_timer;
};

lv_obj_t *create_chart(
  lv_obj_t *parent, 
  const char* id, 
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
  int update_hz);

lv_obj_t** create_grid(lv_obj_t *screen,
    int y_offset,
    int width,
    int height,
    int row_height,
    uint32_t grid_color,
    int total_num_options
);

lv_obj_t** create_grid_with_scroll(lv_obj_t *screen,
    int y_offset,
    int width,
    int height,
    int row_height,
    uint32_t grid_color,
    int total_num_options
);

int dropdown_get_initial_index(
  char *options, 
  const char *initial_value
);

lv_obj_t* create_drop_down(
    lv_obj_t *parent,
    int x_offset,
    char *options,
    int width,
    const char *initial_value
);

lv_obj_t* create_drop_down1(
    lv_obj_t *parent,
    int x_offset,
    char *options,
    int width,
    char *initial_value,
    void (*callback)(lv_event_t*)
);

SwitchWithLabel_T* create_switch_with_label(
    lv_obj_t *parent,
    int width,
    int height,
    int x_offset,
    int option_type,
    bool is_checked,
    int checked_color,
    // lv_style_t *text_style,
    void (*callback)(lv_event_t*)
);

Slider_T* create_slider(
    lv_obj_t *parent,
    int x_offset,
    int label_length,
    int slider_width,
    int min_value,
    int max_value,
    int initial_value,
    int slider_color,
    const char *label_text,
    void (*slider_movement_callback)(lv_event_t*)
);

lv_obj_t * update_selection_pointer(
    lv_obj_t *current_pointer,
    lv_obj_t *new_selection,
    int x_offset,
    lv_style_t *style);

lv_obj_t * update_selection_pointer_color(
    lv_obj_t *current_pointer,
    lv_obj_t *new_selection,
    int x_offset,
    lv_style_t *style
);

void clear_selection_pointer(lv_obj_t **current_pointer);

void on_button_press_navigate_to_screen(const char* json);

lv_obj_t* steno_popup(
    lv_obj_t *parent,
    int width,
    int height,
    int header_height,
    const char *title,
    const char *content
);
