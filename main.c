#include <pebble.h>

#define LEFT_PANEL 34
#define BOTTOM_PANEL 124

Window *my_window;
BitmapLayer *s_red_layer;
BitmapLayer *s_blue_layer;
BitmapLayer *s_yellow_layer;
BitmapLayer *s_line_layer_1;
BitmapLayer *s_line_layer_2;
BitmapLayer *s_line_layer_3;
BitmapLayer *s_line_layer_4;
BitmapLayer *s_line_layer_5;
TextLayer *s_time_layer;
TextLayer *s_date_layer;
GFont s_time_font;
GFont s_date_font;


static void bluetooth_callback(bool connected) {
  if(!connected) {
    // Issue a vibrating alert
    vibes_double_pulse();
  }
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[16];
  static char s_date_buffer[32];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  
  
  strftime(s_date_buffer, sizeof(s_date_buffer), "%a, %d %b", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_date_layer, s_date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  s_time_font = fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS);
  s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_24);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(LEFT_PANEL+5, 63, bounds.size.w-LEFT_PANEL-7, 38));
  
  s_date_layer = text_layer_create(
      GRect(LEFT_PANEL+5, 94, bounds.size.w-LEFT_PANEL-10, 24));
  
  s_red_layer = bitmap_layer_create(
      GRect(LEFT_PANEL, 0, bounds.size.w-LEFT_PANEL, BOTTOM_PANEL));
  bitmap_layer_set_background_color(s_red_layer, GColorRed);
  
  s_blue_layer = bitmap_layer_create(
      GRect(0, BOTTOM_PANEL, LEFT_PANEL, bounds.size.h-BOTTOM_PANEL));
  bitmap_layer_set_background_color(s_blue_layer, GColorBlue);
  
  int tmp = BOTTOM_PANEL+((bounds.size.h-BOTTOM_PANEL) / 2);
  s_yellow_layer = bitmap_layer_create(
      GRect(bounds.size.w-13, tmp, 13, bounds.size.h-tmp));
  bitmap_layer_set_background_color(s_yellow_layer, GColorYellow);
  
  s_line_layer_1 = bitmap_layer_create(
      GRect(LEFT_PANEL-2, 0, 4, bounds.size.h));
  bitmap_layer_set_background_color(s_line_layer_1, GColorBlack);
  s_line_layer_2 = bitmap_layer_create(
      GRect(0, BOTTOM_PANEL-2, bounds.size.w, 4));
  bitmap_layer_set_background_color(s_line_layer_2, GColorBlack);
  s_line_layer_3 = bitmap_layer_create(
      GRect(0, 55, LEFT_PANEL, 6));
  bitmap_layer_set_background_color(s_line_layer_3, GColorBlack);
  s_line_layer_4 = bitmap_layer_create(
      GRect(bounds.size.w-15, BOTTOM_PANEL, 4, bounds.size.h-BOTTOM_PANEL));
  bitmap_layer_set_background_color(s_line_layer_4, GColorBlack);
  s_line_layer_5 = bitmap_layer_create(
      GRect(bounds.size.w-15, tmp-2, 15, 6));
  bitmap_layer_set_background_color(s_line_layer_5, GColorBlack);
  

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorRed);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);
  
  text_layer_set_background_color(s_date_layer, GColorRed);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
  
  bluetooth_callback(connection_service_peek_pebble_app_connection());
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, bitmap_layer_get_layer(s_red_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_blue_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_yellow_layer));
  
  layer_add_child(window_layer, bitmap_layer_get_layer(s_line_layer_1));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_line_layer_2));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_line_layer_3));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_line_layer_4));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_line_layer_5));
  
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  bitmap_layer_destroy(s_red_layer);
  bitmap_layer_destroy(s_blue_layer);
  bitmap_layer_destroy(s_yellow_layer);
  bitmap_layer_destroy(s_line_layer_1);
  bitmap_layer_destroy(s_line_layer_2);
  bitmap_layer_destroy(s_line_layer_3);
  bitmap_layer_destroy(s_line_layer_4);
  bitmap_layer_destroy(s_line_layer_5);
}

void handle_init(void) {
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  connection_service_subscribe((ConnectionHandlers) {
  .pebble_app_connection_handler = bluetooth_callback
});
  
  my_window = window_create();
  
  //window_set_background_color(my_window, GColorRed);
  
  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(my_window, true);
  
  update_time();
}

void handle_deinit(void) {
  window_destroy(my_window);
  bluetooth_connection_service_unsubscribe();
  tick_timer_service_unsubscribe();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
