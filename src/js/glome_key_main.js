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

/**
 * load config page
 */
Pebble.addEventListener("showConfiguration", function(e)
{
  var apiSettings = config.api.settings;
  var fromStorage = localStorage.getItem("apiSettings") || apiSettings;

  try
  {
    apiSettings = JSON.parse(fromStorage);
  }
  catch(e)
  {
    console.log('Parsing local storage data failed: ' + util2.toString(e.message));
  }

  console.log('fromStorage: ' + fromStorage);
  console.log('apiSettings: ' + util2.toString(apiSettings));

  var configPage = 'http://pebble.glome.me/config/glomekey?' +
    'server=' + encodeURI(apiSettings.server) +
    '&apikey=' + apiSettings.apikey +
    '&uid=' + apiSettings.uid;

  console.log('Load config page from: ' + configPage);

  Pebble.openURL(configPage);
});

/**
 * check and save api settings to local storage
 */
Pebble.addEventListener("webviewclosed", function(e)
{
  var value = '';
  var apiSettings = config.api.settings;

  /**
   * the reply should look like something like this:
   * server=xxxx&apikey=xxxx&apiuid=xxxx
   */
  console.log('Raw response from settings page: ' + e.response);

  var reply = e.response.split('&');

  // check, sanitize, always :)
  if (reply.length == 3)
  {
    if (reply[0].indexOf('=') !== -1)
    {
      value = reply[0].split('=')[1];
      if (value != '') apiSettings.server = value;
    }
    if (reply[1].indexOf('=') !== -1)
    {
      value = reply[1].split('=')[1];
      if (value != '') apiSettings.apikey = value;
    }
    if (reply[2].indexOf('=') !== -1)
    {
      value = reply[2].split('=')[1];
      if (value != '') apiSettings.uid = value;
    }

    console.log('Save parsed settings: ' + util2.toString(apiSettings));

    // save to local storage
    localStorage.setItem('apiSettings', util2.toString(apiSettings));

    // send watch a command so that it can recheck the API
    watch.send({'COMMAND_KEY': config.watchCommands.WATCH_CHECK_API_ACCESS});
  }
  else
  {
    console.log('Invalid settings received: ' + e.response);
  }
});
