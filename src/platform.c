/* Includes ----------------------------------------------------------------- */

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#include "platform.h"
#include "constants.h"
#include "sound.h"

/** TODO: remove definition, and make target from makefile */
#define USE_RAYLIB

#ifdef USE_RAYLIB
#include "raylib.h"

/* Definitions -------------------------------------------------------------- */

#define WINDOW_ZOOM 4
#define WINDOW_WIDTH (SCREEN_WIDTH * WINDOW_ZOOM)
#define WINDOW_HEIGHT (SCREEN_HEIGHT * WINDOW_ZOOM)

#define AUDIO_SAMPLING_RATE 44100.0f
#define AUDIO_BUFFER_MAX_SAMPLES 512
#define AUDIO_BUFFER_DEFAULT_SIZE 4096
#define AUDIO_SAMPLE_SIZE 16
#define AUDIO_CHANNEL_NUM 1

/* Global variables --------------------------------------------------------- */

static uint32_t clock_t0;
static uint16_t old_frequency;
static AudioStream audio_stream;

/* Function prototypes ------------------------------------------------------ */

void platform_audio_callback(void *buffer, unsigned int frames);

/* Function definitions ----------------------------------------------------- */

void platform_init(void)
{
    /* Window initialization */
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Doom Pico");
    SetTargetFPS(FPS);

    /* Audio initialization */
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_BUFFER_DEFAULT_SIZE);
    audio_stream = LoadAudioStream(
        AUDIO_SAMPLING_RATE,
        AUDIO_SAMPLE_SIZE,
        AUDIO_CHANNEL_NUM);
    SetAudioStreamCallback(audio_stream, platform_audio_callback);
    PlayAudioStream(audio_stream);
    PauseAudioStream(audio_stream);

    clock_t0 = clock();
}

void platform_draw_start(void)
{
    BeginDrawing();
}

void platform_draw_stop(void)
{
    EndDrawing();
}

void platform_draw_pixel(uint8_t x, uint8_t y, bool color)
{
#if (WINDOW_ZOOM > 1)
    Color c = color ? WHITE : BLACK;
    for (int i = x * WINDOW_ZOOM; i < (x + 1) * WINDOW_ZOOM; i++)
    {
        for (int j = y * WINDOW_ZOOM; j < (y + 1) * WINDOW_ZOOM; j++)
        {
            DrawPixel(i, j, c);
        }
    }
#else
    DrawPixel(x, y, color);
#endif
}

void platform_audio_play(void)
{
    old_frequency = 0;
    ResumeAudioStream(audio_stream);
}

void platform_audio_callback(void *buffer, unsigned int frames)
{
    // Get next frequency to play
    uint16_t frequency = sound_get_frequency();

    // End of sound, pause stream
    if (frequency == 0)
    {
        PauseAudioStream(audio_stream);
        return;
    }

    // Same frequency as before, no need to update
    if (frequency == old_frequency)
        return;

    // Create square wave with given frequency
    uint16_t wave_length = AUDIO_SAMPLING_RATE / frequency;
    uint16_t *buf = (uint16_t *)buffer;
    for (uint16_t i = 0; i < frames; i++)
        buf[i] = (i % wave_length) < wave_length / 2 ? SHRT_MAX : SHRT_MIN;

    old_frequency = frequency;
}

uint32_t platform_millis(void)
{
    return ((clock() - clock_t0) * 1000) / CLOCKS_PER_SEC;
}

void platform_delay(uint32_t ms)
{
    uint32_t t0 = platform_millis();
    while ((platform_millis() - t0) < ms)
    {
        asm("nop");
    };
}

#else

void platform_init(void)
{
}

void platform_draw_start(void)
{
}

void platform_draw_stop(void)
{
}

void platform_draw_pixel(uint8_t x, uint8_t y, bool color)
{
}

void platform_play_audio(void)
{
}

uint32_t platform_millis(void)
{
}

void platform_delay(uint32_t ms)
{
}

#endif /* USE_RAYLIB */

/* -------------------------------------------------------------------------- */