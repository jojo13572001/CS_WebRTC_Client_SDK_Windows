// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#include "stdafx.h"
#include <fstream>
#include "audioframegenerator.h"

AudioFrameGenerator::AudioFrameGenerator(std::string path, int channelNumber, int sampleRate)
{
    path_ = path;
    channelNumber_ = channelNumber;
    sampleRate_ = sampleRate;
    bitsPerSample_ = 2;
    framesize_ = channelNumber_*bitsPerSample_*sampleRate_;
    framesForNext10Ms_ = framesize_ * 10 / 1000;
    fopen_s(&fd, path_.c_str(), "rb");
    if (!fd) {
        std::cout << "Failed to open the " << path_.c_str() << std::endl;
    }
    else {
        std::cout << "Successfully open the " << path_.c_str() << std::endl;
    }
}

AudioFrameGenerator::~AudioFrameGenerator()
{
    fclose(fd);
}

int AudioFrameGenerator::GetSampleRate() {
    return sampleRate_;
}

int AudioFrameGenerator::GetChannelNumber() {
    return channelNumber_;
}

uint32_t AudioFrameGenerator::GenerateFramesForNext10Ms(uint8_t* frame_buffer, const uint32_t capacity)
{
    if (frame_buffer == nullptr) {
        std::cout << "Invaild buffer for frame generator." << std::endl;
        return 0;
    }
    if (framesForNext10Ms_ <= static_cast<int>(capacity)) {
        if (fread(frame_buffer, 1, framesForNext10Ms_, fd) != framesForNext10Ms_)
            fseek(fd, 0, SEEK_SET);
        fread(frame_buffer, 1, framesForNext10Ms_, fd);
    }
    return framesForNext10Ms_;
}

