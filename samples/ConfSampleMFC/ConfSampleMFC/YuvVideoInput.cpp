// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#include <stdafx.h>
#include <iostream>
#include "YuvVideoInput.h"

FileFrameGenerator::FileFrameGenerator(int width, int height, int fps) {
    width_ = width;
    height_ = height;
    type_ = owt::base::VideoFrameGeneratorInterface::I420;
    fps_ = fps;
    int size = width_ * height_;
    int qsize = size / 4;
    frame_data_size_ = size + 2 * qsize;
    fopen_s(&fd, "./source.yuv", "rb");
    if (!fd) {
        std::cout << "failed to open the source.yuv." << std::endl;
    }
    else {
        std::cout << "sucessfully open the source.yuv." << std::endl;
    }
}

FileFrameGenerator::~FileFrameGenerator() {
    fclose(fd);
}

uint32_t FileFrameGenerator::GetNextFrameSize() {
    return frame_data_size_;
}

int FileFrameGenerator::GetHeight() { return height_; }
int FileFrameGenerator::GetWidth() { return width_; }
int FileFrameGenerator::GetFps() { return fps_; }

owt::base::VideoFrameGeneratorInterface::VideoFrameCodec FileFrameGenerator::GetType() { return type_; }

uint32_t FileFrameGenerator::GenerateNextFrame(uint8_t* frame_buffer, const uint32_t capacity) {
    if (capacity < frame_data_size_) {
        return 0;
    }
    if (fread(frame_buffer, 1, frame_data_size_, fd) != (size_t)frame_data_size_) {
        fseek(fd, 0, SEEK_SET);
        fread(frame_buffer, 1, frame_data_size_, fd);
    }
    return frame_data_size_;
}
