// audio_engine.cpp

#include "portaudio.h"
#include <iostream>

extern "C" {

// Audio settings
const int SAMPLE_RATE = 44100;
const int FRAMES_PER_BUFFER = 256;

// Global variables for audio data
float* inputBuffer;
float* outputBuffer;

// PortAudio callback function
static int paCallback(const void* inputBuffer, void* outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void* userData) {
    float* in = (float*)inputBuffer;
    float* out = (float*)outputBuffer;

    for (unsigned int i = 0; i < framesPerBuffer; i++) {
        // Process audio here
        // Example: Echo effect (simple feedback loop)
        out[i] = in[i] + 0.5 * outputBuffer[i];
        // Modify this processing according to your needs
    }

    return 0;
}

// Example: Initialize audio engine
void initialize_audio_engine() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    // Allocate input and output buffers
    inputBuffer = new float[FRAMES_PER_BUFFER];
    outputBuffer = new float[FRAMES_PER_BUFFER];

    // Set up PortAudio stream
    PaStream* stream;
    err = Pa_OpenDefaultStream(&stream, 1, 1, paFloat32, SAMPLE_RATE,
                               FRAMES_PER_BUFFER, paCallback, NULL);

    if (err != paNoError) {
        std::cerr << "PortAudio stream opening failed: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    // Start the stream
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio stream starting failed: " << Pa_GetErrorText(err) << std::endl;
        return;
    }

    std::cout << "Audio engine initialized successfully." << std::endl;
}

}  // extern "C"
