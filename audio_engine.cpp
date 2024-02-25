// audio_engine.cpp

#include "portaudio.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <fstream>

// Logging
#include <iomanip>
#include <ctime>
#include <fstream>

extern "C" {

// Audio settings
const int SAMPLE_RATE = 44100;
const int FRAMES_PER_BUFFER = 256;

// Global variables for audio data
float* inputBuffer;
float* outputBuffer;

// Track structure
struct Track {
    std::vector<float> buffer;
    size_t position;
};

// Vector of tracks representing the mixer
std::vector<Track> mixer;

// PortAudio callback function
static int paCallback(const void* inputBuffer, void* outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void* userData) {
    float* out = (float*)outputBuffer;

    try {
        // Clear the output buffer
        for (unsigned int i = 0; i < framesPerBuffer; i++) {
            out[i] = 0.0f;
        }

        // Mix all tracks
        for (const auto& track : mixer) {
            for (unsigned int i = 0; i < framesPerBuffer; i++) {
                if (track.position < track.buffer.size()) {
                    out[i] += track.buffer[track.position];
                    track.position++;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in callback: " << e.what() << std::endl;
        return paAbort;
    }

    return paContinue;
}

// Example: Initialize audio engine
void initialize_audio_engine() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        throw std::runtime_error("PortAudio initialization failed: " + std::string(Pa_GetErrorText(err)));
    }

    // Allocate input and output buffers
    inputBuffer = new float[FRAMES_PER_BUFFER];
    outputBuffer = new float[FRAMES_PER_BUFFER];

    // Set up PortAudio stream
    PaStream* stream;
    err = Pa_OpenDefaultStream(&stream, 1, 1, paFloat32, SAMPLE_RATE,
                               FRAMES_PER_BUFFER, paCallback, NULL);

    if (err != paNoError) {
        throw std::runtime_error("PortAudio stream opening failed: " + std::string(Pa_GetErrorText(err)));
    }

    // Start the stream
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        throw std::runtime_error("PortAudio stream starting failed: " + std::string(Pa_GetErrorText(err)));
    }

    std::cout << "Audio engine initialized successfully." << std::endl;
}

// Add a track to the mixer
void add_track(const float* audioData, size_t length) {
    Track newTrack;
    newTrack.buffer.assign(audioData, audioData + length);
    newTrack.position = 0;
    mixer.push_back(newTrack);
}

// Clean up resources
void cleanup() {
    delete[] inputBuffer;
    delete[] outputBuffer;
    Pa_Terminate();
    std::cout << "Audio engine terminated." << std::endl;
}

// Example: Load audio from a file
void load_audio_file(const std::string& filename) {
    try {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<float> audioData(size / sizeof(float));
        if (!file.read(reinterpret_cast<char*>(audioData.data()), size)) {
            throw std::runtime_error("Failed to read audio data from file: " + filename);
        }

        add_track(audioData.data(), audioData.size());
        std::cout << "Audio loaded from file: " << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading audio file: " << e.what() << std::endl;
    }
}

}  // extern "C"
