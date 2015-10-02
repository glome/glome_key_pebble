#include <pebble.h>
#include "comm.h"
#include "ui.h"
#include "i18n/en.h"

/**
 * Footer UI element
 */
void create_footer(const char *default_text)
{
  s_footer = text_layer_create(GRect(24, 24, 100, 20));
  text_layer_set_text(s_footer, default_text);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_footer);
}
// Updates a text layer on the UI
void update_footer(const char *text)
{
  text_layer_set_text(s_footer, text);
}

/**
 *
 */
static void initialise_ui(void) {
  s_window = window_create();

  window_set_background_color(s_window, GColorClear);
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, 0);
  #endif

  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(24, 4, 100, 20));
  text_layer_set_text(s_textlayer_1, glome_i18n.main_title);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);

  // create footer UI element
  create_footer(glome_i18n.press_any_key);

  // click handler
  window_set_click_config_provider(s_window, click_config_provider);
}

/**
 *
 */
static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_1);
  text_layer_destroy(s_footer);
}

/**
 *
 */
static void handle_window_unload(Window* window) {
  destroy_ui();
}

/**
 *
 */
void show_glome_key_ui(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

/**
 *
 */
void hide_glome_key_ui(void) {
  window_stack_remove(s_window, true);
}

static void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  update_footer(glome_i18n.pressed_up);
}
/**
 *
 */
static void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  update_footer(glome_i18n.pressed_down);
	send_status(1);
  APP_LOG(APP_LOG_LEVEL_DEBUG, glome_i18n.pressed_down);
}

/**
 *
 */
static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  update_footer(glome_i18n.pressed_select);
  APP_LOG(APP_LOG_LEVEL_DEBUG, glome_i18n.pressed_select);
}

/**
 *
 */
static void select_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
  int size = 50;
  char *buffer = (char *) malloc (size);
  const uint16_t count = click_number_of_clicks_counted(recognizer);

  int res = snprintf(buffer, size, "%s: %d", glome_i18n.pressed_multi_select, count);

  if (res > 0) {
    update_footer(buffer);
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, buffer);
}

/**
 *
 */
static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  update_footer(glome_i18n.pressed_long_select);
  APP_LOG(APP_LOG_LEVEL_DEBUG, glome_i18n.pressed_long_select);
}

/**
 *
 */
static void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, glome_i18n.released_long_select);
}

/**
 *
 */
void click_config_provider(void *context) {
 // single click configs:
  window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);

  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, select_single_click_handler);

  // multi click config:
  window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10, 0, true, select_multi_click_handler);

  // long click config:
  window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
}
