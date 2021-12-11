#pragma once
#include "../bns/FFmpegUtil.h"
#include <memory>

class IMuxer
{
public:
	virtual void WriteVideoPacket(std::shared_ptr<AVPacket> pkt) = 0;
	virtual void WriteAudioPacket(std::shared_ptr<AVPacket> pkt) = 0;
	virtual ~IMuxer() = default;
};