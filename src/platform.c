/* Includes ----------------------------------------------------------------- */

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#include "platform.h"
#include "constants.h"
#include "sound.h"
#include "input.h"

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

/**
 * @brief PLATFORM initialize user-defined functions.
 * 
 */
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

/**
 * @brief PLATFORM start drawing a new frame.
 * 
 */
void platform_draw_start(void)
{
    BeginDrawing();
}

/**
 * @brief PLATFORM stop drawing current frame.
 * 
 */
void platform_draw_stop(void)
{
    EndDrawing();
}

/**
 * @brief PLATFORM write pixel value to screen.
 * 
 * @param x     X coordinate
 * @param y     Y coordinate
 * @param color Pixel color
 */
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

/**
 * @brief PLATFORM play audio effect through speaker.
 * 
 */
void platform_audio_play(void)
{
    old_frequency = 0;
    ResumeAudioStream(audio_stream);
}

/**
 * @brief PLATFORM play audio callback needed for concurrent execution.
 * 
 * NOTE: This callback is specifically required by Raylib, in a microcontroller
 * setting, this callback may be an Interrupt Service Routine (ISR) that
 * autonomously drives the speaker.
 * 
 * @param buffer Sample buffer
 * @param frames Number of samples
 */
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

/**
 * @brief PLATFORM read user controls and update button state.
 * 
 */
void platform_input_update(void)
{
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        input_button = UP;
    else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        input_button = DOWN;
    else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        input_button = LEFT;
    else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        input_button = RIGHT;
    else if (IsKeyDown(KEY_SPACE))
        input_button = Y;
    else if (IsKeyDown(KEY_ESCAPE))
        input_button = SELECT;
}

/**
 * @brief PLATFORM get time in milliseconds from start of execution.
 * 
 * @return uint32_t Start time in milliseconds
 */
uint32_t platform_millis(void)
{
    return ((clock() - clock_t0) * 1000) / CLOCKS_PER_SEC;
}

/**
 * @brief PLATFORM apply blocking delay in milliseconds.
 * 
 * @param ms Delay in milliseconds
 */
void platform_delay(uint32_t ms)
{
    uint32_t t0 = platform_millis();
    while ((platform_millis() - t0) < ms)
    {
        asm("nop");
    };
}

#else /* User-defined platform functions ------------------------------------ */

/**
 * @brief PLATFORM initialize user-defined functions.
 * 
 */
void platform_init(void)
{
    /* Add definition here */
}

/**
 * @brief PLATFORM start drawing a new frame.
 * 
 */
void platform_draw_start(void)
{
    /* Add definition here */
}

/**
 * @brief PLATFORM stop drawing current frame.
 * 
 */
void platform_draw_stop(void)
{
    /* Add definition here */
}

/**
 * @brief PLATFORM write pixel value to screen.
 * 
 * @param x     X coordinate
 * @param y     Y coordinate
 * @param color Pixel color
 */
void platform_draw_pixel(uint8_t x, uint8_t y, bool color)
{
    /* Add definition here */
}

/**
 * @brief PLATFORM play audio effect through speaker.
 * 
 */
void platform_audio_play(void)
{
    /* Add definition here */
}

/**
 * @brief PLATFORM read user controls and update button state.
 * 
 */
void platform_input_update(void)
{
    /* Add definition here */
}

/**
 * @brief PLATFORM get time in milliseconds from start of execution.
 * 
 * @return uint32_t Start time in milliseconds
 */
uint32_t platform_millis(void)
{
    /* Add definition here */
}

/**
 * @brief PLATFORM apply blocking delay in milliseconds.
 * 
 * @param ms Delay in milliseconds
 */
void platform_delay(uint32_t ms)
{
    /* Add definition here */
}
#endif /* USE_RAYLIB */

/* -------------------------------------------------------------------------- */