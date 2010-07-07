#include "VideoDevice.h"
#include "VideoInputSource.h"
#include "logger.h"

#include <algorithm>

namespace sfl {

VideoDevice::VideoDevice(VideoSourceType type,
		std::vector<VideoFormat> formats, const std::string& device,
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
	std::vector<VideoFormat> output;
	std::vector<VideoFormat>::iterator itIn;

	for (itIn = formats.begin(); itIn < formats.end(); itIn++) {
		if ((*itIn).getMimetype() == preferredMimetype) {
			// Always keep native formats, unless it's already present.
			std::vector<VideoFormat>::iterator itOut;
			itOut = std::find(output.begin(), output.end(), (*itIn));

			if (itOut == output.end()) {
				output.push_back((*itIn));
			}
		} else {
			// If not native, only keep if not present.
			std::vector<VideoFormat>::iterator itOut;
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

std::vector<VideoFormat> VideoDevice::getSupportedFormats() const {
	return formats;
}

std::vector<VideoFormat> VideoDevice::getFilteredFormats() const {
	return filteredFormats;
}

void VideoDevice::setPreferredFormat(const VideoFormat& format) {
	preferredFormat = format;
}

VideoFormat VideoDevice::getPreferredFormat() const {
	return preferredFormat;
}

int VideoDevice::getPreferredWidth() const {
	return preferredFormat.getWidth();
}

int VideoDevice::getPreferredHeight() const {
	return preferredFormat.getHeight();
}

int VideoDevice::getPreferredFrameRateNumerator() const {
	return preferredFormat.getPreferredFrameRate().getNumerator();
}

int VideoDevice::getPreferredFrameRateDenominator() const {
	return preferredFormat.getPreferredFrameRate().getDenominator();
}

}
