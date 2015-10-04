#pragma once

void show_glome_key_ui(void);
void hide_glome_key_ui(void);

void click_config_provider(void *context);

void create_header(const char *default_text);
void update_header(const char *text);
void destroy_header();
void create_footer(const char *default_text);
void update_footer(const char *text);
void destroy_header();
void create_qr(char *default_text);
void update_qr(char *text);
void remove_qr();
