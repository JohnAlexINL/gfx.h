Dependencies

**SDL2**
- Gives us our framebuffer
- Handles basic Window IO and window events 

**SDL2Image**
- Gives us image blip and loading

Both could be replaced, but seems unnecessary for now.
The logic in `gfx.h` doesn't care about what kind of renderer it's hooked up to