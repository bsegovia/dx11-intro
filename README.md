Intro "framework"
-----------------

This is a simple source to output a small sized intro using DX11.
This is roughly based on:
* graphicsdemoskeleton here:
  `https://code.google.com/p/graphicsdemoskeleton/source/list` for parts of the
  graphics code
* sound implementation based on IQ code that you can find somewhere here:
  `http://www.iquilezles.org/www/material/isystem1k4k/isystem1k4k.htm`
* crinkler minifying linker here:
  `http://www.crinkler.net/`
* shader minifier here:
 `http://www.ctrl-alt-test.fr/?page_id=7`

The basic idea is to load two shaders (one for sound and one for GFX), compile
them on host and run everything.

Since I was lazy, compilation is using cygwin (calling msvc from a Makefile). It
was just easier to generate the dependency rules with that. There is however a
Visual Studio isolution you can use to debug.

Two outputs are supported:
* 'normal' where you can also hot-reload the shaders
* 'minified' where everything is packed with crinkler and useless code removed

Everything was rushed so expect bugs everywhere.

