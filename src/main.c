#include <pebble.h>
#include <comm.h>
#include <glome/ui.h>

/**
 *
 */
void init(void) {
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);

	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

/**
 *
 */
void deinit(void) {
	app_message_deregister_callbacks();
}

/**
 *
 */
int main( void ) {
	init();
  show_glome_key_ui();

	app_event_loop();

	deinit();
  hide_glome_key_ui();
}
