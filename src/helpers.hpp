
#include <fsdk/FaceEngine.h>

#pragma once
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