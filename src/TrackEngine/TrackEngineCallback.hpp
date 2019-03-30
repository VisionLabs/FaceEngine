#pragma once

class PyICallback {
public:
	enum CallbackType {
		ctVisual,
		ctBestShot,
		ctTrackEnd
	};

	PyICallback(CallbackType _type,
				fsdk::Image _image,
				fsdk::Landmarks5 _landmarks,
				fsdk::Rect _bbox,
				float _score,
				int _trackId,
				int _frameId):
			type{_type},
			image{_image},
			landmarks{_landmarks},
			bbox{_bbox},
			score{_score},
			trackId{_trackId},
			frameId{_frameId}
	{}


	CallbackType type;
	fsdk::Image image;
	fsdk::Landmarks5 landmarks;
	fsdk::Rect bbox;
	float score;
	int trackId;
	int frameId;
};
