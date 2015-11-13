## Glome Key for Pebble

This watch app lets anyone access web sites with a simple one-time key.
There is __no__ need to sign up or register on the web site.

### Screenshots

..todo..

### Try it!

Web sites working with Glome Key:

* http://wp.glome.me

Want to add your site? Let us know: contact@glome.me

### Installation

..todo..

Grab [Glome Key for Pebble](todo) from the Pebble app store using the
Pebble app on your smartphone.

### Build on Linux

#### Requirements

[Pebble SDK](https://developer.getpebble.com/sdk/install/linux/)

#### Instructions

Follow these steps to build the watch app:

<pre>
  git clone git@github.com:glome/glome_key_pebble.git

  cd glome_key_pebble

  git submodule init

  git submodule update

  pebble build
</pre>

The compiled watch app is located at:
<pre>build/glome_key_pebble.pbw</pre>.

### Kudos

* First and foremost thanks to Pebble for the great products!

* Many thanks to Matthew Tole for the qr-layer library:
  https://github.com/matthewtole/qr-layer
