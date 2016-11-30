#include <pebble.h>

//mainwindow
static Window *s_window;

//text layer
static TextLayer *s_text_layer;

//Font style
static GFont s_time_font;

// background
static BitmapLayer *s_background_layer;

static GBitmap *s_background_bitmap;

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Select");
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Up");
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Down");
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
}

static void update_time(){
  time_t temp=time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[8];
  strftime(s_buffer,sizeof(s_buffer),clock_is_24h_style()?
                                                "%H:%M":"%I:%M",tick_time);
  text_layer_set_text(s_text_layer,s_buffer);
}

static void tick_handler(struct tm *tick_time,TimeUnits units_changed){
  update_time();
}



static void prv_window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create GBitmap add before the text Layer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));

  s_text_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(58,52), bounds.size.w, 50));

  text_layer_set_background_color(s_text_layer,GColorClear);
  text_layer_set_text_color(s_text_layer,GColorBlack);

  text_layer_set_text(s_text_layer, "00:00");
  text_layer_set_font(s_text_layer,s_time_font);
  //text_layer_set_font(s_text_layer,fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));


  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);

  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void prv_window_unload(Window *window) {
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);

  // ...
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  // ...
  text_layer_destroy(s_text_layer);
}

static void prv_init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
  tick_timer_service_subscribe(MINUTE_UNIT,tick_handler);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_set_background_color(s_window, GColorBlack);
  const bool animated = true;
  window_stack_push(s_window, animated);
  update_time();
}

static void prv_deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
