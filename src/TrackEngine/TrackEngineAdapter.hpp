#pragma once
#include <string>
#include "../FaceEngineAdapter.hpp"
#include "trackEngine/ITrackEngine.h"
#include "trackEngine/ITrackCallbacks.h"
#include <iostream>

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
		//std::cout << "checkBestShot " << std::endl;
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
	fsdk::Ref<tsdk::IStream> stream;
	Observer streamObserver;
	PyIStream (fsdk::Ref<tsdk::IStream> _stream)
	:stream{_stream}
	{
		stream->setBestShotObserver(&streamObserver);
		//stream->setVisualObserver(&streamObserver);
		//stream->setBestShotPredicate(&streamObserver);
	}

	void pushFrame(const fsdk::Image& image);
};

