#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "gfx.h"
#include "window.h"
#include "image.h"
#include "font.h"

#define framebuffer_width   1920
#define framebuffer_height  1080
unsigned int *framebuffer;

font_t iosevka;
const char iosevka_filename[] = "fonts/iosevka-400-24pt.png";

int         main        ();
void        mainloop    (EVENT *event);
int         file_size   (char *filename);
int         file_read   (char *filename, char *buffer, int max);

void mainloop(EVENT *event)
{
    switch(event->type) {
        case KEYBOARD:      printf("KEYBOARD    %02x (%s)\n", event->c, BOOL_NAMES[event->x]); break;
        case MOUSE:         printf("MOUSE       %02x (%04x, %04x)\n", event->c, event->x, event->y); break;
    }
    gfx_clear( gfx_color_rgb(0,0,0) );
    gfx_setcolor( 0xFFFFFFFF );
    font_puts(&iosevka, "Hello, World!\0", 32, 32);
}

int main () 
{    
    // Initialize our framebuffer
    gfx_color screen[framebuffer_width * framebuffer_height] = { 0 };
    framebuffer = screen; gfx_framebuffer = framebuffer;
    gfx_framebuffer_size = (gfx_vec2){framebuffer_width, framebuffer_height};


    // Initialize our window with our framebuffer
    WINDOW window; 
    int status = window_init(&window, framebuffer, framebuffer_width, framebuffer_height, "TestWindow");
    window_clear(&window, 0,0,0,255);
    window.handler = mainloop;

    // Initialize our font rendering
    if (!font_load(&iosevka, window.renderer, iosevka_filename)) { printf("Failed to set up font renderer\n"); exit(-1); }

    EVENT event = window_eventloop(&window);
}

int file_size(char *filename) { struct stat buffer; return (stat(filename, &buffer) == 0) ? buffer.st_size : -1; }

int file_read(char *filename, char *buffer, int max) {
    FILE *fp = fopen(filename, "rb");
    if ( fp == NULL ) { return false; }
    int cursor = 0; char c; while(cursor < max -1) {
      c = fgetc(fp);
      if( feof(fp) ) { break; }
      buffer[cursor] = c; cursor++;
   } fclose(fp); return cursor;
}