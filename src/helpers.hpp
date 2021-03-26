#pragma once

#include <ErrorsAdapter.hpp>

#include <fsdk/FaceEngine.h>

#include <vector>
#include <tuple>
#include <functional>

#include <pybind11/stl_bind.h> // PYBIND11_MAKE_OPAQUE

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
		if (size > m_size - index)
			return false;
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
		if (size > dataOut.size()-index)
			return false;
		memcpy(data, (void*)&dataOut[0+index], size);
		index += size;
		return true;
	}
	
	void setSizeHint(size_t /*hint*/) noexcept override {}
	
	VectorArchive(std::vector<uint8_t>& inout):
		dataOut(inout)
	{}
};

// only for depth test
fsdk::Image loadImage(const char* name);

std::vector<fsdk::Image> loadFrameSequence(const char*);

std::tuple<DescriptorBatchResult, fsdk::IDescriptorPtr> getDescriptorFromBatch(
	const fsdk::IDescriptorBatchPtr& descriptorBatchPtr,
	int index,
	std::function<fsdk::ResultValue<fsdk::FSDKError, fsdk::IDescriptorPtr>(fsdk::IDescriptorBatchPtr, uint32_t)> func);

std::tuple<FSDKErrorResult, std::vector<FSDKErrorResult>> makeValidationTuple(
	fsdk::Result<fsdk::FSDKError> err, 
	const std::vector<fsdk::Result<fsdk::FSDKError>>& errors);

using MatchingResultList = std::vector<fsdk::MatchingResult>;
PYBIND11_MAKE_OPAQUE(MatchingResultList); //< must be placed before any binding's declaration

