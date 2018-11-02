#include <iostream>
#include <fstream>

#include <fsdk/FaceEngine.h>
#include "helpers.hpp"


int getChannelCount(const fsdk::Format t) {
	switch(t) {
		case fsdk::Format::B8G8R8X8:
			return 4;
		case fsdk::Format::R8G8B8X8:
			return 4;
		case fsdk::Format::B8G8R8:
		case fsdk::Format::R8G8B8:
			return 3;
		case fsdk::Format::R8:
		case fsdk::Format::R16:
			return 1;
		default:
			return 0;
	}
};

fsdk::Image loadImage(const char* name) {
	std::ifstream file(name, std::ios::in|std::ios::binary);
	
	int channels = 0;
	int elementSize = 0;
	int rows = 0;
	int cols = 0;
	
	file.read((char*)&channels, sizeof(channels));
	file.read((char*)&elementSize, sizeof(elementSize));
	file.read((char*)&rows, sizeof(rows));
	file.read((char*)&cols, sizeof(cols));
	
	fsdk::Image image(cols, rows, fsdk::Format::R16);
	
	file.read((char*)image.getData(), rows * cols * channels * elementSize);
	
	return image;
};
