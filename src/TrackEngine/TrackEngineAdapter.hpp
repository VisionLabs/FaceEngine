#pragma once
#include <string>
#include "../FaceEngineAdapter.hpp"
#include "trackEngine/ITrackEngine.h"
#include "trackEngine/ITrackCallbacks.h"
#include "trackEngine/TrackEngineTypes.h"
#include "TrackEngineCallback.hpp"
#include "CallbackObserver.hpp"
#include <iostream>
#include <memory>
#include <list>
#include <vector>
#include <mutex>

class PyIStream;
class PyITrackEngine {
public:
	PyITrackEngine(const PyIFaceEngine& fsdk, const std::string& configPath);
	PyIStream createStream();
private:
	fsdk::Ref<tsdk::ITrackEngine> m_trackEngine;
};

class PyIStream {
public:
	PyIStream(PyIStream& other) = default;

	PyIStream(PyIStream&& other) noexcept:
			m_stream{other.m_stream},
			m_streamObserver{std::move(other.m_streamObserver)}
	{}

	explicit PyIStream(fsdk::Ref<tsdk::IStream>&& _stream);

	bool pushFrame(const fsdk::Image& image, int id);
	std::vector<PyICallback> getCallbacks();
	void waitStream();
private:
	fsdk::Ref<tsdk::IStream> m_stream;
	std::shared_ptr<Observer> m_streamObserver;
	uint32_t m_frameId = 0;
};

