#include "TrackEngineAdapter.hpp"
#include <iostream>

PyITrackEngine::PyITrackEngine(const PyIFaceEngine &fsdk, const std::string &configPath) {
	m_trackEngine = fsdk::acquire(tsdk::createTrackEngine(fsdk.faceEnginePtr.get(), configPath.c_str()));
}

PyIStream PyITrackEngine::createStream() {
	auto stream = fsdk::acquire(m_trackEngine->createStream());
	return PyIStream{std::move(stream)};
}

bool PyIStream::pushFrame(const fsdk::Image &image, int id) {
	return m_stream->pushFrame(image, id);
}

std::vector<PyICallback> PyIStream::getCallbacks() {
	return m_streamObserver->getCallbacks();
}

PyIStream::PyIStream(fsdk::Ref<tsdk::IStream> &&_stream)
		:m_stream{_stream}
{
	if (m_stream.isNull()) {
		std::cout << "error: stream is nullptr!!! " << std::endl;
		return;
	}
	m_streamObserver = std::make_shared<Observer>();
	m_stream->setBestShotObserver(m_streamObserver.get());
	m_stream->setVisualObserver(m_streamObserver.get());
	m_stream->setBestShotPredicate(m_streamObserver.get());
}

void PyIStream::waitStream() {
	m_stream->join();
}
