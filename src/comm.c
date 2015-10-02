#include <pebble.h>
#include <comm.h>

/**
 * Send a Soft Account to PebbleJS
 */
void send_softaccount(const char * const softaccount){
	DictionaryIterator *iter;

	app_message_outbox_begin(&iter);
	dict_write_cstring(iter, SA_KEY, softaccount);

	dict_write_end(iter);
  app_message_outbox_send();
}

/**
 * Send a status to PebbleJS
 */
void send_status(uint8_t status){
	DictionaryIterator *iter;

	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, STATUS_KEY, status);
	dict_write_end(iter);
  app_message_outbox_send();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sent STATUS_KEY: %d", status);
}

/**
 *
 */
char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}

/**
 * Called when a message is received from PebbleKitJS
 */
void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;

	tuple = dict_find(received, SA_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received SA_KEY: %s", tuple->value->cstring);
	}

	tuple = dict_find(received, SYNC_CODE_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received SYNC_CODE_KEY: %s", tuple->value->cstring);
	}

	tuple = dict_find(received, STATUS_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received STATUS_KEY: %d", (int)tuple->value->uint32);
	}
}

/**
 *
 */
void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed to receive message");
}

/**
 *
 */
void out_sent_handler(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message sent");
}

/**
 *
 */
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed sending message, reason: %s", translate_error(reason));
}
