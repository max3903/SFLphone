#include "VideoDevice.h"
#include "VideoInputSource.h"
#include "logger.h"

#include <algorithm>

namespace sfl {

VideoDevice::VideoDevice(VideoSourceType type,
		std::vector<FrameFormat> formats, const std::string& device,
		const std::string& name) throw (InvalidVideoDeviceException) :
	type(type), formats(formats), device(device), name(name) {

	if (formats.size() != 0) {
		// Set the preferred format to be the first in the list. We might also want to sort that one up.
		setPreferredFormat(formats.at(0));
		setPreferredMimetype(VideoInputSource::CLIENT_MIMETYPE);
		filterFrameFormats();
	} else {
		throw InvalidVideoDeviceException(
				"No format was specified in constructing VideoDevice " + name);
	}
}

void VideoDevice::setPreferredMimetype(const std::string& mimetype) {
	preferredMimetype = mimetype;
}

void VideoDevice::filterFrameFormats() {
	std::vector<FrameFormat> output;
	std::vector<FrameFormat>::iterator itIn;

	for (itIn = formats.begin(); itIn < formats.end(); itIn++) {
		if ((*itIn).getMimetype() == preferredMimetype) {
			// Always keep native formats, unless it's already present.
			std::vector<FrameFormat>::iterator itOut;
			itOut = std::find(output.begin(), output.end(), (*itIn));

			if (itOut == output.end()) {
				output.push_back((*itIn));
			}
		} else {
			// If not native, only keep if not present.
			std::vector<FrameFormat>::iterator itOut;
			itOut = std::find(output.begin(), output.end(), (*itIn));

			if (itOut == output.end()) {
				output.push_back((*itIn));
			}
		}
	}

	filteredFormats = output;
}

VideoDevice::VideoDevice(const VideoDevice& other) :
	type(other.getType()), formats(other.getSupportedFormats()), device(
			other.getDevice()), name(other.getName()) {
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

std::vector<FrameFormat> VideoDevice::getFilteredFormats() const {
	return filteredFormats;
}

void VideoDevice::setPreferredFormat(const FrameFormat& format) {
	preferredFormat = format;
}

FrameFormat VideoDevice::getPreferredFormat() const {
	return preferredFormat;
}

int VideoDevice::getPreferredWidth() {
	return preferredFormat.getWidth();
}

int VideoDevice::getPreferredHeight() {
	return preferredFormat.getHeight();
}

int VideoDevice::getPreferredFrameRateNumerator() {
	return preferredFormat.getPreferredFrameRate().getNumerator();
}

int VideoDevice::getPreferredFrameRateDenominator() {
	return preferredFormat.getPreferredFrameRate().getDenominator();
}

}
