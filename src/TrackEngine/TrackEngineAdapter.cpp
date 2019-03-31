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
	return PyIStream{std::move(stream)};
}

bool PyIStream::pushFrame(const fsdk::Image &image, int id) {
	return stream->pushFrame(image, id);
}

std::vector<PyICallback> PyIStream::getCallbacks() {
	return streamObserver->getCallbacks();
}

PyIStream::PyIStream(fsdk::Ref<tsdk::IStream> &&_stream)
		:stream{_stream}
{
	if (stream.isNull()) {
		std::cout << "error: stream is nullptr!!! " << std::endl;
		return;
	}
	streamObserver = std::make_shared<Observer>();
	stream->setBestShotObserver(streamObserver.get());
	stream->setVisualObserver(streamObserver.get());
	stream->setBestShotPredicate(streamObserver.get());
}

void PyIStream::waitStream() {
	stream->join();
}
