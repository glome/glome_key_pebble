#pragma once
/**
 * Dictionary for watch <-> phone (PebbleJS) communication
 */
enum {
  STATUS_KEY = 0,
  SOFTACCOUNT_KEY = 1,
	CODE_KEY = 2,
  COMMAND_KEY = 3,
};

// commands to watch from phone
// see also js/config.json
typedef enum {
  WATCH_LOAD_SOFTACCOUNT = 31,
} WatchCommand;

// commands to phone from watch
// see also js/config.json
typedef enum {
  API_CREATE_SOFTACCOUNT = 51,
  API_CREATE_CODE = 61,
} PhoneCommand;

/**
 * For our persistent storage on the watch
 */
typedef enum {
  GLOME_SOFTACCOUNT = 0,
} PersistentStorage;

/**
 *
 */
typedef struct glomecode {
  uint8_t id;
  char *expires;
  char *value;
} __attribute__((__packed__)) glomecode;

/**
 *
 */
typedef struct glome {
  char *softaccount;
  glomecode *code;
} __attribute__((__packed__)) glome;

//
glome settings;

// Watch -> Phone (PebbleJS)
void send_softaccount(const char *softaccount);
void send_status(uint8_t status);
void send_phone_command(uint8_t command);

// FUTURE: maybe later we can generate unique codes on the watch too
void send_code(glomecode code);

char *translate_error(AppMessageResult result);
// Phone -> Watch
void in_received_handler(DictionaryIterator *received, void *context);
void in_dropped_handler(AppMessageResult reason, void *context);
// Watch -> Phone
void out_sent_handler(DictionaryIterator *iterator, void *context);
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);

const char *load_softaccount();

char *bool2str(bool var);
