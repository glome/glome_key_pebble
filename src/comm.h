#pragma once

/**
 * Dictionary for watch <-> phone (PebbleJS) communication
 */
enum {
	SA_KEY = 0,
	SYNC_CODE_KEY = 1,
  STATUS_KEY = 2
};

/**
 *
 */
typedef struct glome {
  char *softaccount;
} __attribute__((__packed__)) glome;

/**
 * For our persistent storage on the watch
 */
#define GLOME_SOFTACCOUNT 1 // custom key
#define GLOME_SOFTACCOUNT_DEFAULT 'n/a' // default value

// Watch -> Phone (PebbleJS)
void send_softaccount(const char * const softaccount);
void send_status(uint8_t status);
char *translate_error(AppMessageResult result);
// Phone -> Watch
void in_received_handler(DictionaryIterator *received, void *context);
void in_dropped_handler(AppMessageResult reason, void *context);
// Watch -> Phone
void out_sent_handler(DictionaryIterator *iterator, void *context);
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);
