#pragma once

#include "../FaceEngineAdapter.hpp"

class PyICallback {
public:
	enum CallbackType {
		ctVisual,
		ctBestShot,
		ctTrackEnd
	};

	CallbackType type;
	fsdk::Image image;
	fsdk::Landmarks5 landmarks;
	fsdk::Rect bbox;
	float score;
	int trackId;
	int frameId;
	bool isDetection;
};
