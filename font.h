#ifndef FONT_H
#define FONT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct { SDL_Texture *texture; SDL_Renderer *renderer; int width; int height; int kx; int ky; } font_t;

font_t *font_load       (font_t *self, SDL_Renderer * renderer, const char *filename);
void font_putc          (font_t *font, char character, int x, int y);
void font_puts          (font_t *font, char * stream, int x, int y);

font_t *font_load       (font_t *self, SDL_Renderer * renderer, const char *filename)
{
    int x; int y;
    self->renderer = renderer;
    self->texture = IMG_LoadTexture(renderer, (const char *) filename);
    int status = SDL_QueryTexture(
        self->texture,
        NULL, NULL,
        &x, &y
    );
    if ( status != 0 )  { printf("font load: error, could not load file to texture\n"); return NULL; }
    if ( x % 4 != 0 )   { printf("font load: error, width expected to be multiple of four\n"); return NULL; }
    self->width = x;
    self->height = y;
    self->ky = y / 6; // 5++
    self->kx = x / 16;
    // printf("%s\n    kx      %d\n    ky      %d\n    width   %d\n    height  %d\n",
    //     filename, self->kx, self->ky, self->width, self->height
    // );
}

void font_putc          (font_t *font, char character, int x, int y)
{
    character -= ' ';
    int r=0; int c = character;
    while(c > 15) { c -= 16; r++; }
    // printf("putc %c (%d, %d) => R%d, C%d\n", character+' ', x, y, r, c);
    SDL_Rect src =      { c * font->kx, r * font->ky, font->kx, font->ky };
    SDL_Rect dest =     { x, y, font->kx, font->ky };
    int status = SDL_RenderCopy(
        font->renderer,
        font->texture,
        &src,
        &dest
    );
    if (status != 0 ) { printf("font putc: failed to draw character\n"); exit(-1); }
}

void font_puts          (font_t *font, char * stream, int x, int y)
{
    const char maxputs = 100;
    int i=0; for(i=0;i<maxputs;i++) {
        if (stream[i] < ' ' || stream[i] > '~' ) { break; }
        font_putc(font, stream[i], x + (i*font->kx), y);
    }
}

#endif