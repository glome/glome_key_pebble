/**
 * Kudos for Ben Combee
 * http://www.combee.net/resistor-time/config.html
 */
function getParameterByName(name)
{
  name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
  var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
  results = regex.exec(location.search);
  return results === null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
}

// init
jQuery(document).ready(function()
{
  // setup button handler for done
  jQuery("#done").click(function()
  {
    var return_to = getParameterByName("return_to") || "pebblejs://close#";
    var data = 'server=' + jQuery('[name=server]').val();
    data += '&' + 'apikey=' + jQuery('[name=apikey]').val();
    data += '&' + 'uid=' + jQuery('[name=uid]').val();

    (data.length > 9) ? document.location = return_to + data : console.log('fields empty');
  });

  // get default value from the url
  var apiSettings = {
    server: decodeURI(getParameterByName("server")) || 'https://api.glome.me',
    apikey: getParameterByName("apikey") || '',
    uid: getParameterByName("uid") || ''
  }

  console.log('received settings via params: ' + JSON.stringify(apiSettings));

  // fill in the fields
  if (apiSettings.server.length > 0 &&
      apiSettings.apikey.length > 0 &&
      apiSettings.uid.length > 0)
  {
    jQuery('[name=server]').prop('value', apiSettings.server);
    jQuery('[name=apikey]').prop('value', apiSettings.apikey);
    jQuery('[name=uid]').prop('value', apiSettings.uid);
  }
});
