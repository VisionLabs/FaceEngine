#include <fsdk/FaceEngine.h>

#pragma once

struct VectorArchive: fsdk::IArchive
{
	const char* dataOut;
	uint32_t m_size = 0;
	int index = 0;
	
	bool write(const void* data, size_t size) noexcept override {
		memcpy((void*)dataOut, data, size);
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

int getChannelCount(const fsdk::Format t);

// only for depth test
fsdk::Image loadImage(const char* name);