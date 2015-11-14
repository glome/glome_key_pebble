/**
 *
 * Glome HTTP REST API implementation
 *
 */
var util2 = require('util2');
var myutil = require('myutil');
var ajax = require('ajax');

/**
 *
 */
var GlomeAPI = function() {};

/**
 * Prepare API urls
 *
 * * substitutes {glomeid} with real soft account
 * * prepends uri in config.json with server address
 */
GlomeAPI.prototype.prepareApiUrl = function(uri)
{
  var ret = uri;

  if (uri.indexOf('{glomeid}') > 0)
  {
    if (this.glomeId !== null)
    {
      ret = uri.replace('{glomeid}', this.glomeId);
    }
    else
    {
      console.log('Cannot find valid glomeid to be used in the API request.')
      return false;
    }
  }

  var apiSettings = this.config.api.settings;
  // get the server address from storage if available
  var fromStorage = localStorage.getItem("apiSettings");

  try
  {
    apiSettings = JSON.parse(fromStorage);
  }
  catch(e)
  {
    // hard coded default
    console.log('Use default API config');
    console.log(apiSettings);
    //apiSettings = { server: 'https://api.glome.me/' };
  }

  // in any ways we will have a server prop of apiSettings
  ret = apiSettings.server.concat('/' + ret)

  return ret;
};

/**
 * Prepare API credentials from local storage
 */
GlomeAPI.prototype.prepareApiCredentials = function()
{
  var ret = {'application': {'apikey': '', 'uid': ''}};
  var fromStorage = localStorage.getItem("apiSettings") || this.config.api.settings;//{apikey: '', uid: ''};

  try
  {
    var apiSettings = JSON.parse(fromStorage);
    ret.application.apikey = apiSettings.apikey;
    ret.application.uid = apiSettings.uid;
    console.log('Glome API credentials from storage: ' + JSON.stringify(ret));
  }
  catch(e)
  {
    console.log('There are no valid Glome API credentials.');
    return false;
  }

  return ret;
}

/**
 *
 */
GlomeAPI.prototype.init = function(config)
{
  this.config = config;
  // Will be filled in as soon as there is a valid soft account
  this.glomeId = null;

  return true;
}

/**
 * Prepare and send an API request;
 * API implementations should all use this method
 */
GlomeAPI.prototype.request = function (options, success, failure)
{
  // API credentials
  var credentials = this.prepareApiCredentials();

  if (credentials)
  {
    var data = util2.copy(options.data, credentials);
    var url = this.prepareApiUrl(options.uri);

    console.log('send request to url: ' + url);

    if (url == false)
    {
      return url;
    }

    var opts =
    {
      url: url,
      method: options.method,
      data: data,
      type: 'json',
      headers: options.headers,
      async: true
    }

    return ajax(opts, success, failure);
  }
}

/**
 * Our default AJAX failure callback
 * Does nothing, but logs the error
 */
GlomeAPI.prototype.__failureCallback = function(error, status, request)
{
  console.log('The ajax request failed: ' + status);
  return status;
}

/**
 * Implements
 * https://api.glome.me/apidocs/UsersController.html#method-i-create
 */
GlomeAPI.prototype.createSoftAccount = function(options, success, failure)
{
  var _options = util2.copy(this.config.api.createSoftAccount, options);
  var _success = (typeof success !== 'undefined') ? success : function(data, status, request) {
    // data.glomeid is the actual soft account
    return data.glomeid;
  };
  var _failure = (typeof failure !== 'undefined') ? failure : this.__failureCallback;

  return this.request(_options, _success, _failure);
}

/**
 * Implements
 * https://api.glome.me/apidocs/SynchronizationsController.html#method-i-create
 */
GlomeAPI.prototype.createCode = function(options, success, failure)
{
  var _options = util2.copy(this.config.api.createCode, options);
  var _success = (typeof success !== 'undefined') ? success : function(data, status, request) {
    console.log('Created code: ' + data.code);
    return data.code;
  };
  var _failure = (typeof failure !== 'undefined') ? failure : this.__failureCallback;

  return this.request(_options, _success, _failure);
}

module.exports = GlomeAPI;
