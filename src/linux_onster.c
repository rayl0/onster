#include <SDL.h>
#include <stdint.h>

typedef uint8_t bool;

#define true 1
#define false 0

int main(int argc, char *argv[])
{
    SDL_Window *Window = SDL_CreateWindow("onster", 0, 0,
                                          1024, 576,
                                          SDL_WINDOW_SHOWN);

    SDL_Event e;
    bool IsRunning = true;

    while(IsRunning)
    {
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
            case SDL_QUIT:
                IsRunning = false;
                break;
            }
        }
    }

    return 0;
}
