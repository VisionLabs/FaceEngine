#pragma once

#include <fsdk/FaceEngine.h>
#include <vector>

struct Archive: fsdk::IArchive
{
	const char* dataOut;
	uint32_t m_size = 0;
	size_t index = 0;
	
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
	
	Archive(const char* inout, uint32_t size):
	dataOut(inout), m_size(size)
	{}
};

struct VectorArchive: fsdk::IArchive
{
	std::vector<uint8_t>& dataOut;
	size_t index = 0;
	
	bool write(const void* data, size_t size) noexcept override {
		const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
		dataOut.insert(dataOut.end(), p, p+size);
		return true;
	}
	
	bool read(void* data, size_t size) noexcept override {
		assert(size <= dataOut.size()-index);
		memcpy(data, (void*)&dataOut[0+index], size);
		index += size;
		return true;
	}
	
	void setSizeHint(size_t /*hint*/) noexcept override {}
	
	VectorArchive(std::vector<uint8_t>& inout):
		dataOut(inout)
	{}
};


int getChannelCount(const fsdk::Format t);

// only for depth test
fsdk::Image loadImage(const char* name);

std::vector<fsdk::Image> loadFrameSequence(const char*);
