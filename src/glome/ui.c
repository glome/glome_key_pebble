#include <pebble.h>
#include <qr-layer.h>
#include "comm.h"
#include "ui.h"
#include "i18n/en.h"

static Window *s_window;
static TextLayer *s_title;
static TextLayer *s_footer;
static QRLayer *s_qrlayer;
static TextLayer *s_code_1; // the 1st part of the code
static TextLayer *s_code_2; // the 2nd part of the code
static TextLayer *s_code_3; // the 3rd part of the code
static int current_page = 0; // QR code


static PropertyAnimation *s_code_1_animation;
static PropertyAnimation *s_code_2_animation;
static PropertyAnimation *s_code_3_animation;
GRect qr_orig_frame;
GRect code_1_orig_frame;
GRect code_2_orig_frame;
GRect code_3_orig_frame;
GRect code_1_anim_end_frame;
GRect code_2_anim_end_frame;
GRect code_3_anim_end_frame;

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
  // add QR layer to the text layer
  s_qrlayer = qr_layer_create(qr_orig_frame);
  qr_layer_set_data(s_qrlayer, default_text);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_qrlayer);

  // add code bits to the QR layer
  s_code_1 = text_layer_create(code_1_anim_end_frame);
  s_code_2 = text_layer_create(code_2_anim_end_frame);
  s_code_3 = text_layer_create(code_3_anim_end_frame);

  text_layer_set_font(s_code_1, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_font(s_code_2, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_font(s_code_3, fonts_get_system_font(FONT_KEY_GOTHIC_28));

  text_layer_set_text(s_code_1, default_text);
  text_layer_set_text(s_code_2, default_text);
  text_layer_set_text(s_code_3, default_text);

  layer_add_child(s_qrlayer, (Layer *)s_code_1);
  layer_add_child(s_qrlayer, (Layer *)s_code_2);
  layer_add_child(s_qrlayer, (Layer *)s_code_3);
}
// Updates QR code
void update_qr(char *text) {
  if (s_qrlayer) {
    QRData* qr_data = (QRData*) layer_get_data(s_qrlayer);
    if (qr_data && qr_data->width > 0) {
      text_layer_set_text(s_code_1, text);
      text_layer_set_text(s_code_2, text);
      text_layer_set_text(s_code_3, text);
      qr_layer_set_data(s_qrlayer, text);
    }
  }
}
void remove_qr() {
  if (s_qrlayer) {
    QRData* qr_data = (QRData*) layer_get_data(s_qrlayer);
    if (qr_data && qr_data->width > 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Remove QR layer");
      layer_remove_from_parent(s_qrlayer);
    }
  }
}

void code_3_animation_started(Animation *animation, void *data) {
}

void code_3_animation_stopped(Animation *animation, bool finished, void *data) {
  if (current_page == 0) {
    current_page = 1;
  } else {
    current_page = 0;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "QR anim ended. Current page: %d", current_page);
}

/**
 *
 */
static void switch_qr_or_text() {
  // swap s_code and s_qrlayer
  if (current_page == 0) {
    // scroll QR from its place to the left
    s_code_1_animation = property_animation_create_layer_frame((Layer *) s_code_1, &code_1_anim_end_frame, &code_1_orig_frame);
    s_code_2_animation = property_animation_create_layer_frame((Layer *) s_code_2, &code_2_anim_end_frame, &code_2_orig_frame);
    s_code_3_animation = property_animation_create_layer_frame((Layer *) s_code_3, &code_3_anim_end_frame, &code_3_orig_frame);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "QR is shown; switch to code");
  }
  if (current_page == 1) {
    // scroll QR from left to the main area
    s_code_1_animation = property_animation_create_layer_frame((Layer *) s_code_1, &code_1_orig_frame, &code_1_anim_end_frame);
    s_code_2_animation = property_animation_create_layer_frame((Layer *) s_code_2, &code_2_orig_frame, &code_2_anim_end_frame);
    s_code_3_animation = property_animation_create_layer_frame((Layer *) s_code_3, &code_3_orig_frame, &code_3_anim_end_frame);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Code is shown; switch to QR");
  }

  // register anim callbacks
  animation_set_handlers((Animation*) s_code_3_animation, (AnimationHandlers) {
    .started = (AnimationStartedHandler) code_3_animation_started,
    .stopped = (AnimationStoppedHandler) code_3_animation_stopped,
  }, NULL);

  // Schedule to occur ASAP with default settings
  animation_schedule((Animation*) s_code_1_animation);
  animation_schedule((Animation*) s_code_2_animation);
  animation_schedule((Animation*) s_code_3_animation);
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

  qr_orig_frame = GRect(12, 15, 120, 120);
  code_1_orig_frame = GRect(0, 0, 120, 40);
  code_2_orig_frame = GRect(0, 40, 120, 40);
  code_3_orig_frame = GRect(0, 80, 120, 40);
  code_1_anim_end_frame = GRect(-145, 0, 120, 40);
  code_2_anim_end_frame = GRect(-145, 40, 120, 40);
  code_3_anim_end_frame = GRect(-145, 80, 120, 40);

  create_qr(glome_i18n.glome_me);

  create_footer(glome_i18n.press_any_key);

  // the current page shown in the main area of the window
  current_page = 0; // the QR layer

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
  update_footer(glome_i18n.pressed_down);
  // show context aware help page;
  APP_LOG(APP_LOG_LEVEL_DEBUG, glome_i18n.pressed_down);
}

/**
 *
 */
static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  update_footer(glome_i18n.pressed_select);
  // switch between QR layer and a text layer displaying the code
  switch_qr_or_text();
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
