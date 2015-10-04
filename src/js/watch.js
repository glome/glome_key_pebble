/**
 * Simple wrapper to send messages to the watch app
 * using Pebble's AppMessage API
 */
var watch = {};

watch.send = function(json) {
  Pebble.sendAppMessage(
    json,
    function(e) {
      console.log('Successfully delivered message with transactionId='
        + e.data.transactionId + ': ' + JSON.stringify(json));
    },
    function(e) {
      console.log('Unable to deliver message with transactionId='
        + e.data.transactionId
        + ' Error is: ' + e.error.message);
    }
  );
};

module.exports = watch;
