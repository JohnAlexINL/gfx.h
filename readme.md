# Dependencies

**SDL2**
- Gives us our framebuffer
- Handles basic Window IO and window events 

**SDL2Image**
- Gives us image blip and loading

Both could be replaced, but seems unnecessary for now.
The logic in `gfx.h` doesn't care about what kind of renderer it's hooked up to

# API

Include in order:

## gfx.h
```c
// Functions
void            gfx_clear           ();
void            gfx_setcolor        (gfx_color color);
void            gfx_dot             (gfx_vec2 src);
void            gfx_line            (gfx_vec2 src, gfx_vec2 dest);
void            gfx_rect            (gfx_vec4 rect, boolean fill);
void            gfx_triangle        (gfx_vec2 a, gfx_vec2 b, gfx_vec2 c, boolean fill);

// Color Data
gfx_color       gfx_color_rgb       (r,g,b)
gfx_color       gfx_color_rgba      (r,g,b,a)
unsigned char   gfx_color_acomp     (color)
unsigned char   gfx_color_rcomp     (color)
unsigned char   gfx_color_gcomp     (color)
unsigned char   gfx_color_bcomp     (color)

// Typedefs
gfx_color       unsigned int;
gfx_vec2        unsigned int x; unsigned int y;
gfx_vec3        unsigned int x; unsigned int y; unsigned int z;
gfx_vec4        unsigned int x; unsigned int y; unsigned int a; unsigned int b;
gfx_texture     unsigned short width; unsigned short height; gfx_color * data;

// 16.16 Fixed-Point Conversion
gfx_vec2_float      unsigned int
gfx_defloat_vec2    unsigned int
```

## window.h

Make sure to define the Window Event Handler `WINDOW->handler` after initialization and before calling the eventloop

```c
// Options
#define WINDOW_DISABLE_CONTROLLERS

// Functions
int     window_init         (WINDOW *window, void * pixelbuffer, int w, int h, const char *title);
void    window_size         (WINDOW *window, int *w, int *h);
void    window_shutdown     (WINDOW *window);
void    window_present      (WINDOW *window);
void    window_clear        (WINDOW *window, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
EVENT   window_poll         (WINDOW *window);
EVENT   window_eventloop    (WINDOW *window);

// Typedefs
EVENT_HANDLER   void (EVENT *);
EVENT           short type, short x, short y, short c
WINDOW          int w, h, unsigned int *pixels, SDL_Window *, SDL_Renderer *, SDL_Texture *, EVENT_HANDLER handler
```

## font.h
```c
// Functions
font_t *        font_load   (font_t *, SDL_Renderer *, const char *filename);
void            font_putc   (font_t *, char character, int x, int y);
void            font_puts   (font_t *, char * stream, int x, int y);

// Typedefs
font_t          SDL_Texture *, SDL_Renderer *, int width, height, kx, ky
```