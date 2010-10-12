#include "VideoFormat.h"

#include <ostream>
#include <stdlib.h>

#include "logger.h"

namespace sfl
{

const std::string VideoFormat::DEFAULT_MIMETYPE = "video/x-raw-rgb";
const std::string VideoFormat::DEFAULT_FOURCC = "ARGB";
const FrameRate VideoFormat::DEFAULT_FRAMERATE = FrameRate (30, 1);
const int VideoFormat::DEFAULT_WIDTH = 320;
const int VideoFormat::DEFAULT_HEIGHT = 240;

VideoFormat::VideoFormat() throw (InvalidFrameRateException)
{
    framerates.insert (DEFAULT_FRAMERATE);
    init (DEFAULT_MIMETYPE, DEFAULT_FOURCC, DEFAULT_WIDTH, DEFAULT_HEIGHT, framerates);
}

VideoFormat::VideoFormat (const std::string& mimetype, int width, int height, FrameRate framerate) throw (InvalidFrameRateException)
{
    std::set<FrameRate> rates;
    rates.insert (framerate);
    init (mimetype, DEFAULT_FOURCC, width, height, rates);
}

VideoFormat::VideoFormat (const std::string& mimetype, int width, int height,
                          std::set<FrameRate> framerates) throw (InvalidFrameRateException)
{
    init (mimetype, DEFAULT_FOURCC, width, height, framerates);
}

void VideoFormat::init (const std::string& mimetype, const std::string& fourcc, int width, int height,
                        std::set<FrameRate>& framerates) throw (InvalidFrameRateException)
{
    if (framerates.size() == 0) {
        throw InvalidFrameRateException ("No framerate was passed for the given format");
    }

    this->mimetype = mimetype;
    this->fourcc = fourcc;
    this->width = width;
    this->height = height;
    this->framerates = framerates;

    if (framerates.size() == 0) {
        preferredFramerate = DEFAULT_FRAMERATE;
    } else {
        preferredFramerate = * (framerates.rbegin());
    }
}

std::string VideoFormat::getMimetype() const
{
    return mimetype;
}

void VideoFormat::setMimetype (const std::string& mime)
{
    this->mimetype = mime;
}

int VideoFormat::getWidth() const
{
    return width;
}

int VideoFormat::getHeight() const
{
    return height;
}

FrameRate VideoFormat::getPreferredFrameRate() const
{
    return preferredFramerate;
}

std::set<FrameRate> VideoFormat::getFrameRates() const
{
    return framerates;
}

std::string VideoFormat::getFourcc() const
{
    return fourcc;
}

uint32_t VideoFormat::getFourccCode() const {
	if (fourcc == "RGB") {
		return FourCC("RGB3").getValue();
	}

	return FourCC(fourcc).getValue();
}

void VideoFormat::setWidth (const int& width)
{
    this->width = width;
}

void VideoFormat::setHeight (const int& height)
{
    this->height = height;
}

void VideoFormat::setFramerate (const int& numerator, const int& denominator)
{
    _debug ("Setting framerate %d %d", numerator, denominator);
    this->preferredFramerate = FrameRate (numerator, denominator);
}

void VideoFormat::setPreferredFrameRate (const FrameRate& framerate)
throw (InvalidFrameRateException)
{
    preferredFramerate = framerate;
}

void VideoFormat::setFramerate (const std::string& framerate)
{
    size_t pos = framerate.find ("/");
    std::string denominator = framerate.substr (pos+1);
    std::string numerator = framerate.substr (0, pos);

    _debug ("numerator %s denominator %s after parsing", numerator.c_str(), denominator.c_str());

    // TODO Be a bit less naive about the proper syntax.
    setFramerate (atoi (numerator.c_str()), atoi (denominator.c_str()));
}

void VideoFormat::setFourcc (const std::string& fourcc)
{
	this->fourcc = fourcc;
}

void VideoFormat::setFourcc (uint32_t fourcc)
{
    char fourccStr[5];
    fourccStr[0] = fourcc;
    fourccStr[1] = fourcc >> 8;
    fourccStr[2] = fourcc >> 16;
    fourccStr[3] = fourcc >> 24;
    fourccStr[4] = '\0';

	this->fourcc = std::string(fourccStr);
}

void VideoFormat::addFramerate (int numerator, int denominator)
{
    framerates.insert (FrameRate (numerator, denominator));
}

}

std::ostream& operator<< (std::ostream& output, const sfl::VideoFormat& format)
{
    output << format.toString();
    return output;
}

std::ostream& operator<< (std::ostream& output, const sfl::FrameRate& rate)
{
    output << rate.toString();
    return output;
}
