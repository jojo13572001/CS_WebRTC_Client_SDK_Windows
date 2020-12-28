// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#include "stdafx.h"
#include "EncodedVideoInput.h"
#include <stdio.h>
#include <iostream>


using namespace owt::base;

DirectVideoEncoder::DirectVideoEncoder(owt::base::VideoCodec codec) {
    codec_ = codec;
}

DirectVideoEncoder::~DirectVideoEncoder() {
    // Release the encoder resource here.
    if (fd_)
        fclose(fd_);
}

bool DirectVideoEncoder::InitEncoderContext(owt::base::Resolution & resolution, uint32_t fps, uint32_t bitrate, owt::base::VideoCodec video_codec) {
    //Open the resource file.
    std::string videopath = "./source.h264";
    fopen_s(&fd_, videopath.c_str(), "rb");

    if (!fd_) {
        LOG(L"Failed to open the source.h264\n");
    }
    else {
        LOG(L"Successfully open the source.h264\n");
    }
    return true;
}

bool DirectVideoEncoder::EncodeOneFrame(std::vector<uint8_t>& buffer, bool keyFrame) {
    uint32_t frame_data_size;
    if (fread(&frame_data_size, 1, sizeof(int), fd_) != sizeof(int)) {
        fseek(fd_, 0, SEEK_SET);
        fread(&frame_data_size, 1, sizeof(int), fd_);
    }
    uint8_t* data = new uint8_t[frame_data_size];
    fread(data, 1, frame_data_size, fd_);
    buffer.insert(buffer.begin(), data, data + frame_data_size);
    delete[] data;
    return true;
}


DirectVideoEncoder* DirectVideoEncoder::Create(owt::base::VideoCodec codec) {
    DirectVideoEncoder* video_encoder = new DirectVideoEncoder(codec);
    return video_encoder;
}

owt::base::VideoEncoderInterface* DirectVideoEncoder::Copy() {
    DirectVideoEncoder* video_encoder = new DirectVideoEncoder(codec_);
    return video_encoder;
}

bool DirectVideoEncoder::Release() {
    return true;
}
