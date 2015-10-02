#pragma once

static Window *s_window;
static TextLayer *s_textlayer_1;
static TextLayer *s_footer;

void show_glome_key_ui(void);
void hide_glome_key_ui(void);

void click_config_provider(void *context);

void create_footer(const char *default_text);
void update_footer(const char *text);
