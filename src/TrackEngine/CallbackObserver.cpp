#include "CallbackObserver.hpp"

Observer::Observer() {
	m_callbacks = std::make_shared<std::list<PyICallback>>();
}

std::vector<PyICallback> Observer::getCallbacks() {
	std::lock_guard<std::mutex> lock{m_mutex};
	std::vector<PyICallback> ret;
	ret.reserve(m_callbacks->size());

	for (auto& c: *m_callbacks)
		ret.emplace_back(c);

	m_callbacks->clear();
	return ret;
}

void Observer::bestShot(const tsdk::DetectionDescr &detection)  {
	std::lock_guard<std::mutex> lock{m_mutex};
	PyICallback c;
	c.type = PyICallback::ctBestShot;
	c.frameId = detection.frameIndex;
	c.trackId = detection.trackId;
	c.image = detection.image;
	c.bbox = detection.detection.rect;
	c.landmarks = detection.landmarks;
	c.score = detection.detection.score;
	m_callbacks->emplace_back(std::move(c));
}

void Observer::visual(const tsdk::FrameId &frameId, const fsdk::Image &image, const tsdk::TrackInfo *trackInfo,
					  const int nTrack) {
	std::lock_guard<std::mutex> lock{m_mutex};
	for (int i = 0; i < nTrack; i++) {
		PyICallback c;
		c.type = PyICallback::ctVisual;
		c.frameId = frameId;
		c.trackId = trackInfo[i].trackId;
		c.image = image;
		c.bbox = trackInfo[i].rect;
		c.landmarks = trackInfo[i].landmarks;
		c.score = trackInfo[i].lastDetectionScore;
		m_callbacks->emplace_back(std::move(c));
	}
}

void Observer::trackEnd(const tsdk::TrackId &trackId) {
	std::lock_guard<std::mutex> lock{m_mutex};
	PyICallback c;
	c.type = PyICallback::ctTrackEnd;
	c.trackId = trackId;
	m_callbacks->emplace_back(std::move(c));
}

bool Observer::checkBestShot(const tsdk::DetectionDescr &descr) {
	return true;
}