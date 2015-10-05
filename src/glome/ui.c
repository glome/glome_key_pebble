#include <pebble.h>
#include <qr-layer.h>
#include "comm.h"
#include "ui.h"
#include "i18n/en.h"

static Window *window;
static TextLayer *title_layer;
static TextLayer *footer_layer;
static QRLayer *code_qr_layer; // QR code representation of the key code
static TextLayer *code_txt_layer; // textual representation of the key code
static Layer *code_wrapper_layer; // wraps the text layer
static PropertyAnimation *code_wrapper_animation; // for sliding the textual code in / out
static int current_page = 0; // 0: QR code shown; 1: text shown

GRect code_qr_orig_frame;
GRect code_txt_orig_frame;
GRect code_wrapper_orig_frame;
GRect code_wrapper_anim_end_frame;

/**
 * Header UI element aligned to center
 */
void create_header(const char *default_text) {
  title_layer = text_layer_create(GRect(0, 0, 144, 15));
  text_layer_set_text(title_layer, default_text);
  text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), (Layer *)title_layer);
}
// Updates text in the header
void update_header(const char *text) {
  text_layer_set_text(title_layer, text);
}
void destroy_header() {
  text_layer_destroy(title_layer);
}

/**
 * Footer UI element
 */
void create_footer(const char *default_text)
{
  footer_layer = text_layer_create(GRect(0, 133, 144, 15));
  text_layer_set_text(footer_layer, default_text);
  text_layer_set_text_alignment(footer_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), (Layer *)footer_layer);
}
// Updates text in the footer
void update_footer(const char *text) {
  if (text_layer_get_layer(footer_layer)) {
    text_layer_set_text(footer_layer, text);
  }
}
void destroy_footer() {
  if (text_layer_get_layer(footer_layer)) {
    text_layer_destroy(footer_layer);
  }
}

/**
 *
 */
static void code_wrapper_layer_update_proc(Layer* layer, GContext* ctx) {
  GRect layer_size = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, DEFAULT_BG_COLOR);
  graphics_fill_rect(ctx, layer_size, 0, GCornerNone);
  graphics_context_set_fill_color(ctx, DEFAULT_FILL_COLOR);
}

/**
 * QR representation of the key code
 */
void create_qr(char *default_text) {
  // add QR layer first
  code_qr_layer = qr_layer_create(code_qr_orig_frame);
  qr_layer_set_data(code_qr_layer, default_text);
  layer_add_child(window_get_root_layer(window), (Layer *)code_qr_layer);

  // create a wrapper for the text layer (for animation)
  code_wrapper_layer = layer_create(code_wrapper_anim_end_frame);
  layer_set_update_proc(code_wrapper_layer, code_wrapper_layer_update_proc);

  // create a text layer for textual representation of the code
  code_txt_layer = text_layer_create(code_txt_orig_frame);

  text_layer_set_font(code_txt_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text(code_txt_layer, glome_i18n.happy_guy);
  text_layer_set_text_alignment(code_txt_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(code_txt_layer, GTextOverflowModeWordWrap);

  // add a text layer to the wrapper layer (this will be animated)
  layer_add_child((Layer *) code_wrapper_layer, (Layer *) code_txt_layer);
  // add a wrapper layer to the QR layer
  layer_add_child((Layer *) code_qr_layer, (Layer *) code_wrapper_layer);

}
// Updates QR code
void update_qr(char *text) {
  if (code_qr_layer) {
    QRData* qr_data = (QRData*) layer_get_data(code_qr_layer);
    if (qr_data && qr_data->width > 0 && strlen(text) == 12) {
      qr_layer_set_data(code_qr_layer, text);
      // split the text into 4 bytes chunks;
      char code[3][5];
      int i = 0; int j = 0; int k = 0; int res;
      for (i = 1; i < 13; i++) {
        code[j][k] = text[i-1];
        k++;
        if (i%4 == 0)
        {
          code[j][k] = '\0';
          k = 0;
          j = i / 4;
        }
      }
      if (j == 3) {
        // set the code txt layer
        char *txt = "";
        res = snprintf(txt, sizeof(code) + 2, "%s\n%s\n%s", code[0], code[1], code[2]);
        if (res > 0) {
          text_layer_set_text(code_txt_layer, txt);
        }
      }
    }
  }
}
void remove_qr() {
  if (code_qr_layer) {
    QRData* qr_data = (QRData*) layer_get_data(code_qr_layer);
    if (qr_data && qr_data->width > 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Remove QR layer");
      layer_remove_from_parent(code_qr_layer);
    }
  }
}

void code_wrapper_animation_started(Animation *animation, void *data) {
}

void code_wrapper_animation_stopped(Animation *animation, bool finished, void *data) {
  if (current_page == 0) {
    current_page = 1;
  } else {
    current_page = 0;
    //layer_set_hidden((Layer *)s_code_textlayer, true);
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "QR anim ended. Current page: %d", current_page);
}

/**
 *
 */
static void switch_qr_or_text() {
  // swap s_code and code_qr_layer
  if (current_page == 0) {
    // scroll QR from its place to the left
    code_wrapper_animation = property_animation_create_layer_frame((Layer *) code_wrapper_layer, &code_wrapper_anim_end_frame, &code_wrapper_orig_frame);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "QR is shown; switch to code");
  }
  if (current_page == 1) {
    // scroll QR from left to the main area
    code_wrapper_animation = property_animation_create_layer_frame((Layer *) code_wrapper_layer, &code_wrapper_orig_frame, &code_wrapper_anim_end_frame);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Code is shown; switch to QR");
  }

  // register anim callbacks
  animation_set_handlers((Animation*) code_wrapper_animation, (AnimationHandlers) {
    .started = (AnimationStartedHandler) code_wrapper_animation_started,
    .stopped = (AnimationStoppedHandler) code_wrapper_animation_stopped,
  }, NULL);

  // Schedule to occur ASAP with default settings
  animation_schedule((Animation*) code_wrapper_animation);
}

/**
 *
 */
static void initialise_ui(void) {
  window = window_create();

  window_set_background_color(window, GColorClear);
  #ifndef PBL_SDK_3
    window_set_fullscreen(window, 0);
  #endif

  // basic UI parts
  create_header(glome_i18n.main_title);

  code_qr_orig_frame = GRect(12, 15, 120, 120);
  code_wrapper_orig_frame = GRect(0, 0, 120, 120); // relative to the qr frame
  code_wrapper_anim_end_frame = GRect(0, 0, 0, 120);
  code_txt_orig_frame = GRect(0, 10, 120, 110); // relative to the wrapper frame
  create_qr(glome_i18n.glome_me);

  create_footer(glome_i18n.press_any_key);

  // the current page shown in the main area of the window
  current_page = 0; // the QR layer

  // click handler
  window_set_click_config_provider(window, click_config_provider);
}

/**
 *
 */
static void destroy_ui(void) {
  window_destroy(window);
  destroy_header();
  destroy_footer();
  qr_layer_destroy(code_qr_layer);
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
  window_set_window_handlers(window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(window, true);
}

/**
 *
 */
void hide_glome_key_ui(void) {
  window_stack_remove(window, true);
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
