#pragma once
#include <string>
#include "../FaceEngineAdapter.hpp"
#include "trackEngine/ITrackEngine.h"
#include "trackEngine/ITrackCallbacks.h"
#include "trackEngine/TrackEngineTypes.h"
#include "TrackEngineCallback.hpp"
#include <iostream>
#include <memory>
#include <list>
#include <vector>
#include <mutex>

using CallbacksListPtr =  std::shared_ptr<std::list<PyICallback>>;

struct Observer :
		tsdk::IBestShotObserver,
		tsdk::IVisualObserver,
		tsdk::IBestShotPredicate {
	void bestShot(const tsdk::DetectionDescr& detection) override {

	}

	void visual(const tsdk::FrameId &frameId,
				const fsdk::Image &image,
				const tsdk::TrackInfo * trackInfo,
				const int nTrack) override {
		std::cout << "visual" << std::endl;
		/*std::cout << "visual frame " << frameId << std::endl;
		for (size_t i = 0; i < nTrack; i++) {
			std::cout << trackInfo[i].rect.x << " "
			<< trackInfo[i].rect.y << " "
			<< trackInfo[i].rect.width << " "
			<< trackInfo[i].rect.height << std::endl;
		}*/
	}

	void trackEnd(const tsdk::TrackId& trackId) override {
		//std::cout << "track Finished " << trackId << std::endl;
	}

	bool checkBestShot(const tsdk::DetectionDescr& descr) override {
		return true;
	}
};

class PyIStream;
class PyITrackEngine {
public:
	fsdk::Ref<tsdk::ITrackEngine> trackEngine;

	PyITrackEngine(const PyIFaceEngine& fsdk, const std::string& configPath);
	std::string getStr();
	std::string getFSDKVer(const PyIFaceEngine& fsdk);
	PyIStream createStream();
};

class PyIStream {
public:
	PyIStream(PyIStream& other):
			stream{other.stream},
			streamObserver{other.streamObserver},
			callbacks{other.callbacks}
	{}

	PyIStream(PyIStream&& other) noexcept:
			stream{std::move(other.stream)},
			streamObserver{std::move(other.streamObserver)},
			callbacks{std::move(other.callbacks)}
	{}

	explicit PyIStream(fsdk::Ref<tsdk::IStream>&& _stream)
	:stream{_stream}
	{
		if (stream.isNull()) {
			std::cout << "error: stream is nullptr!!! " << std::endl;
			return;
		}
		callbacks = std::make_shared<std::list<PyICallback>>();
		streamObserver = std::make_shared<Observer>();
		stream->setBestShotObserver(streamObserver.get());
		stream->setVisualObserver(streamObserver.get());
		stream->setBestShotPredicate(streamObserver.get());
	}

	void pushFrame(const fsdk::Image& image);
	std::vector<PyICallback> getCallbacks();

	std::mutex m_mutex;
	fsdk::Ref<tsdk::IStream> stream;
	std::shared_ptr<Observer> streamObserver;
	CallbacksListPtr callbacks;
};

