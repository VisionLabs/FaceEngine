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
	Observer();

	void bestShot(const tsdk::DetectionDescr& detection) override;

	void visual(const tsdk::FrameId &frameId,
				const fsdk::Image &image,
				const tsdk::TrackInfo * trackInfo,
				const int nTrack) override;

	void trackEnd(const tsdk::TrackId& trackId) override;

	bool checkBestShot(const tsdk::DetectionDescr& descr) override;
	std::vector<PyICallback> getCallbacks();
private:
	CallbacksListPtr m_callbacks;
	std::mutex m_mutex;
};
