#include "VideoDevice.h"
#include "logger.h"

namespace sfl {
VideoDevice::VideoDevice(VideoSourceType type,
		std::vector<FrameFormat> formats, const std::string& device, const std::string& name) throw(InvalidVideoDeviceException) :
	type(type), formats(formats), device(device), name(name) {

	if (formats.size() != 0) {
		// Set the preferred format to be the first in the list. We might also want to sort that one up.
		setPreferredFormat(formats.at(0));
	} else {
		throw InvalidVideoDeviceException("No format was specified in constructing VideoDevice " + name);
	}
}

VideoSourceType VideoDevice::getType() const {
	return type;
}

std::string VideoDevice::getDevice() const {
	return device;
}

std::string VideoDevice::getName() const {
	return name;
}

std::vector<FrameFormat> VideoDevice::getSupportedFormats() const {
	return formats;
}

void VideoDevice::setPreferredFormat(const FrameFormat& format) {
	preferredFormat = format;
}

FrameFormat VideoDevice::getPreferredFormat() const {
	return preferredFormat;
}

int VideoDevice::getPreferredWidth()
{
	return preferredFormat.getWidth();
}

int VideoDevice::getPreferredHeight()
{
	return preferredFormat.getHeight();
}

int VideoDevice::getPreferredFrameRateNumerator()
{
	return preferredFormat.getPreferredFrameRate().getNumerator();
}

int VideoDevice::getPreferredFrameRateDenominator()
{
	return preferredFormat.getPreferredFrameRate().getDenominator();
}

}
