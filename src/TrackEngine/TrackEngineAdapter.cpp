#include "TrackEngineAdapter.hpp"
#include <iostream>

std::string PyITrackEngine::getStr() {
	return "PyITrackEngine str";
}

std::string PyITrackEngine::getFSDKVer(const PyIFaceEngine &fsdk) {
	std::cout << "getFSDKVer c++" << std::endl;
	auto edition = fsdk.faceEnginePtr->getFaceEngineEdition();
	if (edition == fsdk::FaceEngineEdition::CompleteEdition)
		return "Complete";
	return "Frontend";
}

PyITrackEngine::PyITrackEngine(const PyIFaceEngine &fsdk, const std::string &configPath) {
	trackEngine = fsdk::acquire(tsdk::createTrackEngine(fsdk.faceEnginePtr.get(), configPath.c_str()));
}

PyIStream PyITrackEngine::createStream() {
	auto stream = fsdk::acquire(trackEngine->createStream());
	return PyIStream{stream};
}

void PyIStream::pushFrame(const fsdk::Image &image) {
	static uint32_t n = 0;
	stream->pushFrame(image, n++);
}
