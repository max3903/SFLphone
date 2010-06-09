#include "FrameFormat.h"

#include <ostream>
#include <stdlib.h>

#include "logger.h"

namespace sfl {

const char* FrameFormat::DEFAULT_MIMETYPE = "video/x-raw-rgb";
const FrameRate FrameFormat::DEFAULT_FRAMERATE = FrameRate(30, 1);

FrameFormat::FrameFormat() throw (InvalidFrameRateException) {
	framerates.insert(DEFAULT_FRAMERATE);
	init(DEFAULT_MIMETYPE, DEFAULT_WIDTH, DEFAULT_HEIGHT, framerates);
}

FrameFormat::FrameFormat(const std::string& mimetype, int width, int height,
		std::set<FrameRate> framerates) throw (InvalidFrameRateException) {
	init(mimetype, width, height, framerates);
}

void FrameFormat::init(const std::string& mimetype, int width, int height,
		std::set<FrameRate>& framerates) throw (InvalidFrameRateException) {
	if (framerates.size() == 0) {
		throw InvalidFrameRateException("No framerate was passed for the given format");
	}

	this->mimetype = mimetype;
	this->width = width;
	this->height = height;
	this->framerates = framerates;
	preferredFramerate = DEFAULT_FRAMERATE;
}

void FrameFormat::setWidth(const int& width)
{
	this->width = width;
}

void FrameFormat::setHeight(const int& height)
{
	this->height = height;
}

void FrameFormat::setFramerate(const int& numerator, const int& denominator)
{
	_debug("Setting framerate %d %d", numerator, denominator);
	this->preferredFramerate = FrameRate(numerator, denominator);
}

void FrameFormat::setFramerate(const std::string& framerate)
{
	size_t pos = framerate.find("/");
	std::string denominator = framerate.substr(pos+1);
	std::string numerator = framerate.substr(0, pos);

	_debug("numerator %s denominator %s after parsing", numerator.c_str(), denominator.c_str());

	// TODO Be a bit less naive about the proper syntax.
	setFramerate(atoi(numerator.c_str()), atoi(denominator.c_str()));
}

void FrameFormat::addFramerate(int numerator, int denominator) {
	framerates.insert(FrameRate(numerator, denominator));
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
