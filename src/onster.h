#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ONSTER_ENSURE(x, msg, ...)                                    \
    {                                                               \
        if(!(x)) { fprintf(stderr, msg, __VA_ARGS__); *(u8*)0 = 1;} \
    }

#if defined(ONSTER_DEBUG)
#define Assert(x) ONSTER_ENSURE(x, "Assertion failed: %s\n In File: %s at line: %i\n", #x, __FILE__, __LINE__);
#else
#define Assert(x)
#endif

// Used in places using malloc and os memory
#define MEMORY_LEAK

// Used in places using manual printf's
#define TODO_LOGGING

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef int8_t s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;

    typedef u8 ubyte;
    typedef s8 byte;
    typedef u8 b32; // NOTE(rajat): Using 8-bit booleans

    typedef float r32;
    typedef double r64;

    extern void OnsterInit();

    typedef struct onster_button
    {
        b32 EndedDown;
    }onster_button;

    typedef struct onster_input
    {
        struct
        {
            onster_button MoveUp;
            onster_button MoveDown;
            onster_button MoveLeft;
            onster_button MoveRight;

            onster_button A;
            onster_button B;
            onster_button X;
            onster_button Y;

            onster_button Terminate;
        }Buttons;
    }onster_input;

    extern void OnsterUpdateAndRender(onster_input *Input, r32 dt);

    struct file_params
    {
        u32 Size;
        b32 Exists;
    };

    extern file_params
    GetFileParams(char* Path);

    // NOTE(rajat): Might want some file streaming functions

    extern s32
    LoadFile(char* Path, char* Buffer);

    extern s32
    LoadFileRaw(char* Path, void* Buffer);
#ifdef __cplusplus
}
#endif
