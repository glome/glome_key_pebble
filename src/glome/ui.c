#include <pebble.h>
#include <qr-layer.h>
#include "comm.h"
#include "ui.h"
#include "i18n/en.h"

static Window *s_window;
static TextLayer *s_title;
static TextLayer *s_footer;
static QRLayer *s_qrlayer;

/**
 * Header UI element aligned to center
 */
void create_header(const char *default_text) {
  s_title = text_layer_create(GRect(0, 0, 144, 15));
  text_layer_set_text(s_title, default_text);
  text_layer_set_text_alignment(s_title, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_title);
}
// Updates text in the header
void update_header(const char *text) {
  text_layer_set_text(s_title, text);
}
void destroy_header() {
  text_layer_destroy(s_title);
}

/**
 * Footer UI element
 */
void create_footer(const char *default_text)
{
  s_footer = text_layer_create(GRect(0, 133, 144, 15));
  text_layer_set_text(s_footer, default_text);
  text_layer_set_text_alignment(s_footer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_footer);
}
// Updates text in the footer
void update_footer(const char *text) {
  if (text_layer_get_layer(s_footer)) {
    text_layer_set_text(s_footer, text);
  }
}
void destroy_footer() {
  if (text_layer_get_layer(s_footer)) {
    text_layer_destroy(s_footer);
  }
}

/**
 * QR UI element
 */
void create_qr(char *default_text) {
  s_qrlayer = qr_layer_create(GRect(12, 15, 120, 120));
  qr_layer_set_data(s_qrlayer, default_text);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_qrlayer);
}
// Updates QR code
void update_qr(char *text) {
  if (s_qrlayer) {
    QRData* qr_data = (QRData*) layer_get_data(s_qrlayer);
    if (qr_data && qr_data->width > 0) {
      qr_layer_set_data(s_qrlayer, text);
    }
  }
}
void remove_qr() {
  if (s_qrlayer) {
    //qr_layer_destroy(s_qrlayer);
    TextLayer *white = text_layer_create(GRect(0, 15, 144, 140));
    text_layer_set_text_alignment(white, GTextAlignmentCenter);
    text_layer_set_text(white, "?");
    //BitmapLayer *white = bitmap_layer_create(GRect(0, 15, 144, 140));
    layer_add_child(window_get_root_layer(s_window), (Layer *)white);
  }

  //layer_mark_dirty(qr_layer_get_layer(s_qrlayer));
  //bool hidden = (layer_get_hidden(s_qrlayer) != true);

  //layer_set_hidden(s_qrlayer, hidden);

  //if (hidden) {
//    update_qr("");
  //}

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "QR layer hidden: %s", bool2str(hidden));
  //layer_remove_from_parent(s_qrlayer);
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

  // basic UI parts
  create_header(glome_i18n.main_title);
  create_qr(glome_i18n.glome_me);
  create_footer(glome_i18n.press_any_key);

  // click handler
  window_set_click_config_provider(s_window, click_config_provider);
}

/**
 *
 */
static void destroy_ui(void) {
  window_destroy(s_window);
  destroy_header();
  destroy_footer();
  qr_layer_destroy(s_qrlayer);
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
  send_phone_command(API_CREATE_CODE);
  APP_LOG(APP_LOG_LEVEL_DEBUG, glome_i18n.pressed_up);
}
/**
 *
 */
static void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  remove_qr();
  update_footer(glome_i18n.pressed_down);
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
