#ifndef WINDOW_H
#define WINDOW_H

// #define WINDOW_DISABLE_CONTROLLERS disables controller support and controller-based features

#include <SDL2/SDL.h>
#include <time.h>

/* ------------ PROTOTYPES AND FUNCTION INTERFACES --------- */

typedef struct  { uint16_t type; uint16_t x; uint16_t y; uint16_t c; } EVENT;
typedef void    (*EVENT_HANDLER)(EVENT *);
typedef struct  { int w, h; unsigned int *pixels; SDL_Window *window; SDL_Renderer *renderer; SDL_Texture *texture; EVENT_HANDLER handler; } WINDOW;

static inline int       window_init         (WINDOW *window, void * pixelbuffer, int w, int h, const char *title);
static inline void      window_size         (WINDOW *window, int *w, int *h);
static inline void      window_shutdown     (WINDOW *window);
static inline void      window_present      (WINDOW *window);
static inline void      window_clear        (WINDOW *window, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
static inline EVENT     window_poll         (WINDOW *window);
static inline EVENT     window_eventloop    (WINDOW *window);

// ------------ GLOBAL VARIABLES --------------------------- //

// ------------ FUNCTION IMPLEMENTATIONS ------------------- //

void timer_wait () 
{
    SDL_Delay(0);
}

int timer(double seconds) 
{
    static clock_t timer_last = 0;
    clock_t now = clock();
    double elapsed = (double)(now - timer_last) / CLOCKS_PER_SEC;
    if ( elapsed >= seconds || timer_last == 0 ) {
        timer_last = now; return true;
    }   return false;

}

enum {
    ERROR, NONE, KEYBOARD, MOUSE, 
    CONNECT, BUTTON, JOYSTICK
};

const char EVENT_NAMES[][10] = {
    "NONE", 
    "KEYBOARD", "MOUSE",
    #ifndef WINDOW_DISABLE_CONTROLLERS
        "CONNECT", "BUTTON", "JOYSTICK",
    #endif
};

const char BOOL_NAMES[][8] = {
    [true] = "true",
    [false] = "false"
};

#ifndef WINDOW_DISABLE_CONTROLLERS
enum { PS4_X,  PS4_O,  PS4_S,  PS4_T, PS4_SHARE, PS4_HOME, PS4_OPT, PS4_LS, PS4_RS, PS4_LB, PS4_RB, PS4_UP, PS4_DOWN, PS4_LEFT, PS4_RIGHT, PS4_LCLICK };
const char PS4_BUTTON_NAMES[][7] = {
    [PS4_X]         = "X",  
    [PS4_O]         = "O",  
    [PS4_S]         = "S",  
    [PS4_T]         = "T", 
    [PS4_SHARE]     = "SHARE",
    [PS4_HOME]      = "HOME", 
    [PS4_OPT]       = "OPT", 
    [PS4_LS]        = "LS", 
    [PS4_RS]        = "RS", 
    [PS4_LB]        = "LB", 
    [PS4_RB]        = "RB", 
    [PS4_UP]        = "UP", 
    [PS4_DOWN]      = "DOWN", 
    [PS4_LEFT]      = "LEFT", 
    [PS4_RIGHT]     = "RIGHT", 
    [PS4_LCLICK]    = "LCLICK"
};
#endif

static inline int window_init(WINDOW *window, void * pixelbuffer, int w, int h, const char *title)
{
    window->w = w;
    window->h = h;
    window->handler = NULL;

    window->pixels = pixelbuffer;
    if (!window->pixels) { return -1; }
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) { return -1; }
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) { return -1; } 
    
    SDL_JoystickEventState(SDL_ENABLE);

    window->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        w, h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window->window)
        return -1;

    window->renderer = SDL_CreateRenderer(window->window, -1, 0);
    if (!window->renderer)
        return -1;

    window->texture = SDL_CreateTexture(
        window->renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        w, h
    );
    if (!window->texture)
        return -1;

    return 0;
}

static inline void window_size(WINDOW *window, int *w, int *h)
{
    SDL_GetWindowSize(window->window, w, h);
}

static inline void window_shutdown(WINDOW *window)
{
    if (window->texture) SDL_DestroyTexture(window->texture);
    if (window->renderer) SDL_DestroyRenderer(window->renderer);
    if (window->window) SDL_DestroyWindow(window->window);
    SDL_Quit();
    free(window->pixels);
}

static inline void window_present(WINDOW *window)
{
    SDL_UpdateTexture(window->texture, NULL, window->pixels, window->w * sizeof(unsigned int));
    SDL_RenderCopy(window->renderer, window->texture, NULL, NULL); // copy texture first
    SDL_RenderPresent(window->renderer);                       // then present
}

static inline void window_clear(WINDOW *window, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if ( a != 255 ) { SDL_SetWindowOpacity( window->window, (float)(a)/(float)255 ); }
    unsigned int color = (r << 16) | (g << 8) | b;
    for (int i = 0; i < window->w * window->h; ++i)
        window->pixels[i] = color;
}

static inline EVENT window_poll(WINDOW *window) 
{

    #ifndef WINDOW_DISABLE_CONTROLLERS
        static SDL_Joystick *controller[8] = { NULL };
        const int max_controllers = sizeof(controller)/sizeof(controller[0]); int controllers = 0;
    #endif

    SDL_Event e; // https://wiki.libsdl.org/SDL2/SDL_Event
    while (SDL_PollEvent(&e)) {
    switch(e.type) {
            // <Alt+F4> Exit
        case SDL_QUIT: {
            window_shutdown(window); exit(0);
            break;
        }
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            // <Alt+Enter> Toggle Fullscreen
            if (e.key.keysym.sym == SDLK_RETURN && (e.key.keysym.mod & KMOD_ALT) && e.key.state == SDL_PRESSED )
            {
                Uint32 flags = SDL_GetWindowFlags(window->window);
                if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) { SDL_SetWindowFullscreen(window->window, 0); }
                else { SDL_SetWindowFullscreen(window->window, SDL_WINDOW_FULLSCREEN_DESKTOP); }
                break;
            }
            // Otherwise, process the keystroke and return it
            else {
                return (EVENT) { .type = KEYBOARD, .x = (e.key.state == SDL_PRESSED ), .c = e.key.keysym.sym, };
            }
        }
        case SDL_MOUSEMOTION: {
            uint16_t my = e.motion.y; 
            uint16_t mx = e.motion.x;
            return (EVENT) { .type = MOUSE, .x = mx, .y = my, .c = false };
        }
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            uint16_t my = e.button.y;
            uint16_t mx = e.button.x;
            uint16_t c  = e.button.button;
            if ( e.button.state == SDL_RELEASED ) { c = c | 128;  }
            return (EVENT) { .type = MOUSE, .x = my, .y = my, .c = c };
        }

        #ifndef WINDOW_DISABLE_CONTROLLERS
            // Controller Connect and Disconnect
        case SDL_JOYDEVICEADDED: {
            if ( controllers >= max_controllers ) { break; } // no more than max
            SDL_Joystick *self = SDL_JoystickOpen(e.jdevice.which);
            if (self) { controller[controllers++] = self; }
            return (EVENT) { .type = CONNECT, .c = controllers-1 };
            break;
        }
        case SDL_JOYDEVICEREMOVED: {
            SDL_JoystickID index = e.jdevice.which;
            for (int i = 0; i < controllers; i++) {
                if (SDL_JoystickInstanceID(controller[i]) == index) {
                    SDL_JoystickClose(controller[i]);
                    controller[i] = controller[--controllers];
                    controller[controllers] = NULL;
                    break;
                }
            }
            return (EVENT) { .type = CONNECT, .c = -1 };
            break;
        }
            // Controller Sticks
        case SDL_JOYAXISMOTION: {
            uint16_t source = ( e.jaxis.which ) << 4;
            uint16_t degree = e.jaxis.value;
            uint16_t axis;
            uint16_t mx = 0; uint16_t my = 0;
            // Map each stick to its own "source"
            axis = e.jaxis.axis / 2;
            // Denormalize trigger axises
            if ( axis >= 2 ) { degree -= 0x8000; }
            // Map the stick value based on input axis
            if ( e.jaxis.axis % 2 == 0 ) { mx = degree; } else { my = degree; }
            return (EVENT) { .type = JOYSTICK, .c = source + axis, .x = mx, .y = my };
        }
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP: {
            uint16_t source = ( e.jbutton.which ) << 4;
            uint16_t button =   e.jbutton.button;
            uint16_t state =    e.jbutton.state;
            return (EVENT) { .type = BUTTON, .c = source, .x = button, .y = (state == SDL_PRESSED) };
        }
        #endif
    }}
}

static inline EVENT window_eventloop (WINDOW *window)
{
    EVENT yield;
    if ( window->handler == NULL ) { return (EVENT) { .type = ERROR }; }
    for(;;) {
        yield = window_poll(window);
        if (yield.type != NONE ) { window->handler(&yield); }
        SDL_RenderPresent(window->renderer);
        if ( timer( (float)(1.0 / 300.0) ) == false ) { timer_wait(); continue; }
        SDL_Delay(0);
    }
}

#endif