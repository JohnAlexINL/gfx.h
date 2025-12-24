#ifndef GFX_HEADER
#define GFX_HEADER
#define gfx_max                         -1
#define gfx_bytes_short                 2
#define gfx_bytes_long                  4

typedef unsigned int                    gfx_unitL;
typedef unsigned short                  gfx_unitS;
typedef gfx_unitL                       gfx_unitF;
typedef unsigned int                    boolean;
typedef boolean                         bool;
enum { false, true };

#define cast_char_unitS(value)          ( value & 0xFF ), ( (value>>8)& 0xFF )
#define cast_char_unitL(value)          ( value & 0xFF ), ( (value>>8)& 0xFF ), ( (value>>16)& 0xFF ), ( (value>>24)& 0xFF ),

//      RRGGBBAA 32-bit
#define gfx_color_rgb(r,g,b)            gfx_color_rgba(r, g, b, gfx_max)
#define gfx_color_rgba(r,g,b,a)         ( ((a & 0xFF) << 24 ) | ((r & 0xFF) << 16 ) | ((g & 0xFF) << 8 ) | ( b & 0xFF ))
#define gfx_color_acomp(color)          ( (color >> 24) & 0xFF )
#define gfx_color_rcomp(color)          ( (color >> 16) & 0xFF )
#define gfx_color_gcomp(color)          ( (color >> 8) & 0xFF )
#define gfx_color_bcomp(color)          ( (color) & 0xFF )

#define gfx_vec2_float(vector)          ( (gfx_vec2){ vector.x << 16, vector.y << 16 } )
#define gfx_defloat_vec2(vector)        ( (gfx_vec2){ vector.x >> 16, vector.y >> 16 } )

typedef gfx_unitL                                                       gfx_color;
typedef struct { gfx_unitL x; gfx_unitL y; }                            gfx_vec2;
typedef struct { gfx_unitL x; gfx_unitL y; gfx_unitL z; }               gfx_vec3;
typedef struct { gfx_unitL x; gfx_unitL y; gfx_unitL a; gfx_unitL b; }  gfx_vec4;
typedef struct { gfx_unitS width; gfx_unitS height; gfx_color data[0];} gfx_texture;

void            gfx_clear           ();
void            gfx_setcolor        (gfx_color color);
void            gfx_dot             (gfx_vec2 src);
void            gfx_line            (gfx_vec2 src, gfx_vec2 dest);
void            gfx_rect            (gfx_vec4 rect, boolean fill);
void            gfx_triangle        (gfx_vec2 a, gfx_vec2 b, gfx_vec2 c, boolean fill);

gfx_color *     gfx_framebuffer =                                       NULL;
gfx_vec2        gfx_framebuffer_size =                                  { 0 };
gfx_color       gfx_currentColor =                                      0xFFFFFF;

inline void     gfx_clear ()
{
    int i; for(i=0;i<gfx_framebuffer_size.x * gfx_framebuffer_size.y;i++) { gfx_framebuffer[i] = gfx_currentColor; }
}

inline void     gfx_setcolor        (gfx_color color)
{
    gfx_currentColor = color;
}

void            gfx_dot             (gfx_vec2 src) 
{
    if ( src.x >= gfx_framebuffer_size.x || src.y >= gfx_framebuffer_size.y ) { return; }
    gfx_framebuffer[ src.y * gfx_framebuffer_size.x + src.x ] = gfx_currentColor;
}

void            gfx_line            (gfx_vec2 src, gfx_vec2 dest)
{
    int dx = dest.x - src.x;
    int dy = dest.y - src.y;
    float slope, b;
    int x, y;
    // Sweep along Y
    if (dy != 0) {
        int stepY = (dy > 0) ? 1 : -1;
        slope = (float)(dx) / (float)(dy); // dx/dy
        b = src.x - slope * src.y;
        for (y = src.y; y != dest.y + stepY; y += stepY) {
            x = (int)(slope * y + b + 0.5f);
            gfx_dot((gfx_vec2){x, y});
        }
    }
    // Sweep along X
    if (dx != 0) {
        int stepX = (dx > 0) ? 1 : -1;
        slope = (float)(dy) / (float)(dx); // dy/dx
        b = src.y - slope * src.x;
        for (x = src.x; x != dest.x + stepX; x += stepX) {
            y = (int)(slope * x + b + 0.5f);
            gfx_dot((gfx_vec2){x, y});
        }
    }
}


void            gfx_rect            (gfx_vec4 rect, boolean fill)
{
    if ( fill == false ) {
        gfx_line( (gfx_vec2){ rect.x, rect.y }, (gfx_vec2){ rect.x, rect.b } ); // LEFT
        gfx_line( (gfx_vec2){ rect.a, rect.y }, (gfx_vec2){ rect.a, rect.b } ); // RIGHT
        gfx_line( (gfx_vec2){ rect.x, rect.y }, (gfx_vec2){ rect.a, rect.y } ); // TOP
        gfx_line( (gfx_vec2){ rect.x, rect.b }, (gfx_vec2){ rect.a, rect.b } ); // BOTTOM
        return;
    }
    int y; for(y=rect.y;y<rect.b;y++) {
        gfx_line( (gfx_vec2){ rect.x, y }, (gfx_vec2){ rect.a, y} );
    }
}

void            gfx_triangle        (gfx_vec2 a, gfx_vec2 b, gfx_vec2 c, boolean fill)
{
    if (!fill) {
        gfx_line(a, b);
        gfx_line(b, c);
        gfx_line(c, a);
        return;
    }

    // TODO: Implement filled triangles
    // We quite honestly can probably use <https://wiki.libsdl.org/SDL2/SDL_RenderGeometry>

}

#endif