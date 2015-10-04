/**
 * Glome Key - Pebble phone app
 *
 * This code will run on the phone within the MyPebble app's sandbox
 *
 */
var util2 = require("util2");
var myutil = require("myutil");
var watch = require("watch");
var config = require("config");
var GlomeAPI = require("glome_api");
var glomeApi = new GlomeAPI();

// Called when JS is ready
Pebble.addEventListener("ready",
  function(e) {
    console.log('Pebble app ready');

    var ret = glomeApi.init(config);
    if (typeof ret !== 'undefined')
    {
      // load soft account from watch
      watch.send({'COMMAND_KEY': config.watchCommands.WATCH_LOAD_SOFTACCOUNT});
    }
});

// Called when incoming message from the watch app is received
Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received from watch: " + util2.toString(e.payload));

    if (typeof e.payload.COMMAND_KEY !== 'undefined')
    {
      switch(e.payload.COMMAND_KEY)
      {
        case config.apiCommands.API_CREATE_SOFTACCOUNT:
          var success = function(data, status, request) {
            // send the received soft account to the watchapp
            watch.send({'SOFTACCOUNT_KEY': data.glomeid});
            //store it for using other API calls
            glomeApi.glomeId = data.glomeid;

            console.log('Created soft account: ' + data.glomeid);
          };
          glomeApi.createSoftAccount(null, success, null);
          break;
        case config.apiCommands.API_CREATE_CODE:
          var success = function(data, status, request) {
            console.log('Created code: ' + data.code);
            // send the received code to the watchapp
            watch.send({'CODE_KEY': data.code});
          };
          glomeApi.createCode(null, success, null);
          break;
        default:
          console.log("Unknown API command: " + util2.toString(e.payload.COMMAND_KEY));
      }
    }
    if (typeof e.payload.CODE_KEY != 'undefined')
    {
      // todo
    }
    if (typeof e.payload.SOFTACCOUNT_KEY !== 'undefined' &&
        e.payload.SOFTACCOUNT_KEY != '')
    {
      console.log("Soft account received from watch: " + e.payload.SOFTACCOUNT_KEY);
      glomeApi.glomeId = e.payload.SOFTACCOUNT_KEY;
    }
});
