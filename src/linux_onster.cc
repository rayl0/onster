// TODO
// -- Input Button Maps (NOTE(rajat): Game's frontend should handle this with native gui
// or with some callback)

// NOTE(rajat): I will not provide or create a function
// if it is being or will being called once at any specific
// point in the codebase. I will pull out that function if there
// is need to call it somewhere elese.

// NOTE(rajat): Same for data bundles, I will not create a struct
// if only an single instance of it is being used or if it is not passed
// between API's or different subsystems of same the program.

#include <SDL.h>
#include <stdint.h>
#include <x86intrin.h>

#include "onster.h"

// NOTE(rajat): Backend implementations
#include "onster_gl.cc"

#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

file_params
GetFileParams(char* Path)
{
    file_params Params;

    struct stat FileState;
    stat(Path, &FileState);

    Params.Size = FileState.st_size;

    s32 FileHandleTemp;
    FileHandleTemp = open(Path, O_RDONLY);
    Params.Exists = (FileHandleTemp != -1) ? 1 : 0;
    close(FileHandleTemp);

    return Params;
}

s32
LoadFileRaw(char* Path, void* Buffer)
{
    if(Buffer)
    {
        s32 FileHandle;
        FileHandle = open(Path, O_RDONLY);

        Assert(FileHandle != -1);

        struct stat FileState;
        stat(Path, &FileState);

        if(FileState.st_size == 0)
            return -1;

        read(FileHandle, Buffer, FileState.st_size);

        return 0;
    }

    return -1;
}

s32
LoadFile(char* Path, char* Buffer)
{
    s32 err = LoadFileRaw(Path, Buffer);

    struct stat FileState;
    stat(Path, &FileState);

    if(!err)
        Buffer[FileState.st_size] = '\0';

    return err;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *Window = SDL_CreateWindow("onster", 0, 0,
                                          1024, 576,
                                          SDL_WINDOW_SHOWN |
                                          SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GLContext Context = SDL_GL_CreateContext(Window);
    SDL_GL_MakeCurrent(Window, Context);

#ifndef GL_ES
    gladLoadGL();
#endif

    SDL_Event e;
    b32 IsRunning = true;

    printf("%s\n", glGetString(GL_VERSION));

    onster_input Input[2] = {};
    onster_input *OldInput = &Input[0];
    onster_input *NewInput = &Input[1];

    SDL_GL_SetSwapInterval(0);

    u64 PerfCountFrequency = SDL_GetPerformanceFrequency();
    u64 LastCycleCount = _rdtsc();
    u64 LastCounter = SDL_GetPerformanceCounter();

    r64 LastMSPerFrame = {};

    while(IsRunning)
    {
        NewInput->Buttons = {};

        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {

            case SDL_QUIT: {
                IsRunning = false;
                NewInput->Buttons.Terminate.EndedDown = true;
            }   break;
            case SDL_KEYDOWN: {
                switch(e.key.keysym.sym)
                {
                case SDLK_a:
                    NewInput->Buttons.A.EndedDown = true;
                    break;
                case SDLK_s:
                    NewInput->Buttons.B.EndedDown = true;
                    break;
                case SDLK_UP:
                    NewInput->Buttons.MoveUp.EndedDown = true;
                    break;
                case SDLK_DOWN:
                    NewInput->Buttons.MoveDown.EndedDown = true;
                    break;
                case SDLK_LEFT:
                    NewInput->Buttons.MoveLeft.EndedDown = true;
                    break;
                case SDLK_RIGHT:
                    NewInput->Buttons.MoveRight.EndedDown = true;
                    break;
                }
            }   break;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1, 0, 0, 1);

        OnsterUpdateAndRender(NewInput, LastMSPerFrame);
        SDL_GL_SwapWindow(Window);

        onster_input *Temp = NewInput;

        NewInput = OldInput;
        OldInput = Temp;

        u64 EndCycleCount = _rdtsc();
        u64 EndCounter = SDL_GetPerformanceCounter();

        r64 CyclesElapsed = EndCycleCount - LastCycleCount;
        r64 CounterElapsed = EndCounter - LastCounter;

        LastCycleCount = EndCycleCount;
        LastCounter = EndCounter;
        r64 MSPerFrame = (( 1000.0f * (r64)CounterElapsed) / (r64)PerfCountFrequency);
        r64 FPS = (r64)PerfCountFrequency / (r64)CounterElapsed;
        r64 MCPF = ((r64)CyclesElapsed / (1000.0f * 1000.0f));

        LastMSPerFrame = MSPerFrame;
#if 1
        printf("%.02fms/f, %.02ff/s, %.02fmc/f\n", MSPerFrame, FPS, MCPF);
#endif

    }

    return 0;
}
