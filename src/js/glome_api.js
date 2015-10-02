/**
 *
 * Glome HTTP REST API implementation
 *
 */
var ajax = require('ajax');
var util2 = require('util2');
var Settings = require('settings');

/**
 *
 */
var GlomeAPI = function()
{
  this.config = {
    // change if needed
    'server': 'http://10.0.1.101:3000/',
    'apikey': '85f3a11f14ffcd4f0e2091701a473792',
    'uid': 'me.glome.p1'
  };

  this.defaults = {
    // settings key for soft account
    'SA_KEY': 'glome_sa',
    // api entry points
    'api_create_sa': 'users.json'
  };

  this.init();
}

/**
 *
 */
GlomeAPI.prototype.init = function()
{
  // load SA from storage
  this.softAccount = this.getSoftAccountFromSettings();
  console.log('this.softAccount = ' + this.softAccount);

  // no SA yet
  // line 243 in overall
  if (! this.softAccount || typeof this.softAccount == 'undefined')
  {
    console.log('No SA yet');
    this.createSoftAccount();
  }

  return true;
}

/**
 *
 */
GlomeAPI.prototype.request = function (options, success, failure)
{
  // prepend data payload with API credentials
  var credentials = {
    'application' :
    {
      'apikey': this.config.apikey,
      'uid': this.config.uid
    }
  };

  var data = util2.copy(options.data, credentials);

  var opts =
  {
    url: this.config.server.concat(options.api_entry),
    method: options.method,
    data: data,
    type: 'json',
    headers: options.headers,
    async: true
  }

  return ajax(opts, success, failure);
}

/**
 *
 */
GlomeAPI.prototype.getSoftAccountFromSettings = function()
{
  var self = this;

  if (! Settings || ! Settings.option(self.defaults.SA_SETTINGS_KEY))
  {
    console.log('No settings object or no SA saved to settings yet.');
    return false;
  }
  console.log(0);
  console.log('Send this: ' + Settings.option(self.defaults.SA_SETTINGS_KEY));
  console.log(1);

  this.sendMessage({'SA_KEY': Settings.option(self.defaults.SA_SETTINGS_KEY), 'STATUS_KEY': 111});

  return Settings.option(self.defaults.SA_SETTINGS_KEY);
}

/**
 *
 */
GlomeAPI.prototype.saveSoftAccountToSettings = function(softAccount)
{
  if (! Settings || ! softAccount)
  {
    console.log('No settings object or no SA to be saved to settings.');
    return false;
  }
  Settings.option(this.defaults.SA_SETTINGS_KEY, softAccount);
  console.log('Successfuly saved: ' + this.defaults.SA_SETTINGS_KEY + ' => ' + softAccount);

  //this.sendAppMessage({'SA_KEY': Settings.option(this.defaults.SA_SETTINGS_KEY), 'SYNC_CODE_KEY': 1});

  return Settings.option(this.defaults.SA_SETTINGS_KEY);
}

/**
 *
 */
GlomeAPI.prototype.createSoftAccount = function()
{
  var self = this;
  var options = {};

  options.api_entry = this.defaults.api_create_sa;
  options.method = 'POST';

  success = function(data, status, request)
  {
    //var result = JSON.parse(body);
    console.log('Created SoftAccount: ' + data.glomeid);
    self.saveSoftAccountToSettings(data.glomeid);
    return data.glomeid;
  },
  failure = function(error, status, request)
  {
    console.log('The ajax request failed: ' + status);
    return status;
  }

  return this.request(options, success, failure);
}

GlomeAPI.prototype.sendMessage = function(msg)
{
  // send a message, so that we can catch the message / event
  // and react to it in app.js
  console.log('Have to send this: ' + JSON.stringify(msg));
  var transactionId = Pebble.sendAppMessage(msg);
    //~ function(e) {
      //~ console.log('Successfully delivered message with transactionId ='
        //~ + e.data.transactionId);
    //~ },
    //~ function(e) {
      //~ console.log('Unable to deliver message with transactionId ='
        //~ + e.data.transactionId
        //~ + ' Error is: ' + e.error.message);
    //~ }
  //~ );

  return transactionId;
}

module.exports = GlomeAPI;
