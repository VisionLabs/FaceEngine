#include <iostream>

#include <fsdk/FaceEngine.h>
#include "helpers.hpp"

fsdk::Image loadImage(const char* name) {
	std::ifstream file(name, std::ios::in|std::ios::binary);
	
	int channels = 0;
	int elementSize = 0;
	int rows = 0;
	int cols = 0;
	
	file.read((char*)&channels, sizeof(channels));
	file.read((char*)&elementSize, sizeof(elementSize));
	file.read((char*)&rows, sizeof(rows));
	file.read((char*)&cols, sizeof(cols));
	
	fsdk::Image image(cols, rows, fsdk::Format::R16);
	
	file.read((char*)image.getData(), rows * cols * channels * elementSize);
	
	return image;
};


std::vector<fsdk::Image> loadFrameSequence(const char* path){
	std::vector<fsdk::Image> sequence;
	
	std::ifstream file;
	file.open(path, std::ios::in | std::ios::binary);
	if(!file.is_open()) return sequence;
	
	int frames = 0;
	int width = 0;
	int height = 0;
	int type = 0;
	
	file.read((char*)&frames, sizeof(frames));
	file.read((char*)&width, sizeof(width));
	file.read((char*)&height, sizeof(height));
	file.read((char*)&type, sizeof(type));
	
	sequence.reserve(frames);
	
	fsdk::Image frame(width, height, static_cast<fsdk::Format::Type>(type));
	auto size = frame.getDataSize();
	
	for (int i = 0; i < frames; ++i){
		file.read((char*)frame.getData(), size);
		sequence.emplace_back(frame.clone());
	}
	
	return sequence;
}

bool saveFrameSequence(const std::vector<fsdk::Image>& sequence, std::string path){
	
	if(sequence.empty()) return false;
	
	std::ofstream file;
	file.open(path, std::ios::out | std::ios::binary);
	if(!file.is_open()) return false;
	
	size_t frames = sequence.size();
	int width = sequence[0].getWidth();
	int height = sequence[0].getHeight();
	int type = fsdk::Format::Type(sequence[0].getFormat());
	
	for (auto frame : sequence)
		if (!(width == frame.getWidth() &&
		height == frame.getHeight() &&
		type == fsdk::Format::Type(frame.getFormat())))
			return false;
	
	file.write((char*)&frames, sizeof(frames));
	file.write((char*)&width, sizeof(width));
	file.write((char*)&height, sizeof(height));
	file.write((char*)&type, sizeof(type));
	
	for (uint32_t i = 0; i < frames; ++i){
		auto size = sequence[i].getDataSize();
		file.write((char*)sequence[i].getData(), size);
	}
	return true;
}

std::tuple<DescriptorBatchResult, fsdk::IDescriptorPtr> getDescriptorFromBatch(
	const fsdk::IDescriptorBatchPtr &descriptorBatchPtr,
	int index,
	std::function<fsdk::ResultValue<fsdk::FSDKError, fsdk::IDescriptorPtr>(fsdk::IDescriptorBatchPtr, uint32_t)> func) {
	
		if (!descriptorBatchPtr) {
			return std::make_tuple(DescriptorBatchResult(
				fsdk::makeResult(fsdk::IDescriptorBatch::Error::InvalidInput)), fsdk::IDescriptorPtr());
		}
		
		if ((uint32_t)index < 0 || (uint32_t)index >= descriptorBatchPtr->getCount()) {
			return std::make_tuple(DescriptorBatchResult(
				fsdk::makeResult(fsdk::IDescriptorBatch::Error::OutOfRange)), fsdk::IDescriptorPtr());
		}
	
		fsdk::ResultValue<fsdk::FSDKError, fsdk::IDescriptorPtr> resDescriptor = func(descriptorBatchPtr, index);
		
		if (resDescriptor) {
			return std::make_tuple(DescriptorBatchResult(
				fsdk::makeResult(fsdk::IDescriptorBatch::Error::Ok)), resDescriptor.getValue());
		} else {
			return std::make_tuple(DescriptorBatchResult(
				fsdk::makeResult(fsdk::IDescriptorBatch::Error::Internal)), fsdk::IDescriptorPtr());
		}
}

std::tuple<FSDKErrorResult, std::vector<FSDKErrorResult>>
	makeValidationTuple(
		fsdk::Result<fsdk::FSDKError> err, 
		const std::vector<fsdk::Result<fsdk::FSDKError>>& errors) {

		if (err.getError() == fsdk::FSDKError::InvalidSpanSize || err.getError() == fsdk::FSDKError::InvalidBatch) {
			return std::make_tuple(FSDKErrorResult(err), std::vector<FSDKErrorResult>());
		}

		return std::make_tuple(FSDKErrorResult(err), std::vector<FSDKErrorResult>(errors.begin(), errors.end()));
	}