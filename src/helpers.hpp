
#include <fsdk/FaceEngine.h>

#pragma once

auto getChannelCount = [](fsdk::Format t) {
	switch(t) {
		case fsdk::Format::B8G8R8X8:
			return 4;
		case fsdk::Format::R8G8B8X8:
			return 4;
		case fsdk::Format::B8G8R8:
		case fsdk::Format::R8G8B8:
			return 3;
		case fsdk::Format::R8:
		case fsdk::Format::R16:
			return 1;
		default:
			return 0;
	}
};

struct VectorArchive: fsdk::IArchive
{
	const char* dataOut;
	uint32_t m_size = 0;
	int index = 0;
	
	bool write(const void* data, size_t size) noexcept override {
		memcpy((void*)&dataOut, data, size);
		return true;
	}
	
	bool read(void* data, size_t size) noexcept override {
		assert(size <= m_size - index);
		memcpy(data, (void*)&dataOut[index], size);
		index += size;
		return true;
	}
	
	VectorArchive(const char* inout, uint32_t size):
	dataOut(inout), m_size(size)
	{}
};

