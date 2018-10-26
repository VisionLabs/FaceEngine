
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
	std::vector<uint8_t>& dataOut;
	int index = 0;
	
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

constexpr int sizeOfBatch = 999;

struct ProgressTracker: fsdk::IProgressTracker {
	mutable std::array<float, sizeOfBatch / 100> reports;
	mutable size_t sz = 0;
	
	virtual void progress(const float completion) const noexcept final {
		printf("Progress: %f", completion);
		reports[sz++] = completion;
	}
};