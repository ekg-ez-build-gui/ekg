# EKG
EKG now supports Windows/Linux, the project is totally built in a context SDL2 and OpenGL, be sure you have setup correctly, also if you are looking for ![EKG-API-docs](https://github.com/ekg-ez-build-gui/ekg-api-docs/) page.

In the code you look something like that:
```
/* Start of something. */
ekg_do_something();
/* End of something. */
```
I use this style code to find easy something in a big file (if you think, is easy to write start of... end of...).

EKG uses `SDL_image` `SDL_ttf` to handler font and images file.
For implement EKG in your project first you need to add all these libs.
