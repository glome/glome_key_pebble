// Function to send a message to the Pebble using AppMessage API
function sendMessage() {
	Pebble.sendAppMessage(
    {"STATUS_KEY": 1},
    function(e) {
      console.log('Successfully delivered message with transactionId='
        + e.data.transactionId);
    },
    function(e) {
      console.log('Unable to deliver message with transactionId='
        + e.data.transactionId
        + ' Error is: ' + e.error.message);
    }
  );
};

// Called when JS is ready
Pebble.addEventListener("ready",
  function(e) {
    console.log('Pebble app ready');
    sendMessage();
});

// Called when incoming message from the Pebble is received
Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received appmessage: " + JSON.stringify(e.payload));
});
