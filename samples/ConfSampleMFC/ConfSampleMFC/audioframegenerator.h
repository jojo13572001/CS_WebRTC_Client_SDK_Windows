// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#pragma once
#include "owt/base/framegeneratorinterface.h"
#include <stdio.h>
#include <iostream>

class AudioFrameGenerator : public owt::base::AudioFrameGeneratorInterface {
public:
    explicit AudioFrameGenerator(std::string path, int channelNumber, int sampleRate);
    virtual ~AudioFrameGenerator();
    virtual uint32_t GenerateFramesForNext10Ms(uint8_t* frame_buffer, const uint32_t capacity);
    virtual int GetSampleRate() override;
    virtual int GetChannelNumber() override;
    std::string path_;

private:
    FILE * fd;
    int channelNumber_;
    int sampleRate_;
    int bitsPerSample_;
    int framesize_;
    int framesForNext10Ms_;
};
