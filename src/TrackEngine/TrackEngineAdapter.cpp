#include "TrackEngineAdapter.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;

PyITrackEngine::PyITrackEngine(const PyIFaceEngine &fsdk, const std::string &configPath) {
	auto res = tsdk::createTrackEngine(fsdk.faceEnginePtr.get(), configPath.c_str());
	if (res.isError()) {
		const std::string message = std::string("\nFailed to create TrackEngine instance! See the "
			"\"Troubleshooting and diagnostics\" chapter in the documentation "
			"for possible reasons. Error: ") + res.what();
		throw py::cast_error(message.c_str());
	}
	m_trackEngine =res.getValue();
}

PyIStream PyITrackEngine::createStream() {
	auto stream = fsdk::acquire(m_trackEngine->createStream());
	return PyIStream{std::move(stream)};
}

bool PyIStream::pushFrame(const fsdk::Image &image, int id) {
	return m_stream->pushFrame(image, id, nullptr);
}

std::vector<PyICallback> PyIStream::getCallbacks() {
	return m_streamObserver->getCallbacks();
}

PyIStream::PyIStream(fsdk::Ref<tsdk::IStream> &&_stream)
		:m_stream{_stream}
{
	if (m_stream.isNull())
		throw py::cast_error("\nPyIStream error: stream is nullptr!");
	m_streamObserver = std::make_shared<Observer>();
	m_stream->setBestShotObserver(m_streamObserver.get());
	m_stream->setVisualObserver(m_streamObserver.get());
	m_stream->setBestShotPredicate(m_streamObserver.get());
}

void PyIStream::waitStream() {
	m_stream->join();
}
