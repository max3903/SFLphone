#include "FrameFormat.h"
#include <ostream>

namespace sfl {

const char* FrameFormat::DEFAULT_MIMETYPE = "video/x-raw-rgb";
const FrameRate FrameFormat::DEFAULT_FRAMERATE = FrameRate(30, 1);

FrameFormat::FrameFormat() throw (InvalidFrameRateException) {
	std::vector<FrameRate> framerates;
	framerates.push_back(DEFAULT_FRAMERATE);
	init(DEFAULT_MIMETYPE, DEFAULT_WIDTH, DEFAULT_HEIGHT, framerates);
}

FrameFormat::FrameFormat(const std::string& mimetype, int width, int height,
		std::vector<FrameRate> framerates) throw (InvalidFrameRateException) {
	init(mimetype, width, height, framerates);
}

void FrameFormat::init(const std::string& mimetype, int width, int height,
		std::vector<FrameRate>& framerates) throw (InvalidFrameRateException) {
	if (framerates.size() == 0) {
		throw InvalidFrameRateException("No framerate was passed for the given format");
	}

	this->mimetype = mimetype;
	this->width = width;
	this->height = height;
	this->framerates = framerates;
	preferredFramerate = framerates.at(0);
}

void FrameFormat::addFramerate(int numerator, int denominator) {
	framerates.push_back(*(new FrameRate(numerator, denominator)));
}

void FrameFormat::setPreferredFrameRate(const FrameRate& framerate)
		throw (InvalidFrameRateException) {
	preferredFramerate = framerate;
}

FrameRate FrameFormat::getPreferredFrameRate() {
	return preferredFramerate;
}

}

std::ostream& operator<<(std::ostream& output, const sfl::FrameFormat& format) {
    output << format.toString();
    return output;
}

std::ostream& operator<<(std::ostream& output, const sfl::FrameRate& rate) {
    output << rate.toString();
    return output;
}
