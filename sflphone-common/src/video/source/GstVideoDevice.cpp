#include "GstVideoDevice.h"
#include "../logger.h"

#include <sstream>

namespace sfl {
GstVideoDevice::GstVideoDevice(VideoSourceType type,
		std::vector<VideoFormat> formats, const std::string& device,
		const std::string& name) :
	VideoDevice(type, formats, device, name) {
}

GstVideoDevice::GstVideoDevice(const GstVideoDevice& other) :
	VideoDevice(other.getType(), other.getSupportedFormats(),
			other.getDevice(), other.getName()), gstreamerPipeline(other.getGstPipeline()) {
}

std::string GstVideoDevice::getGstPipeline() const {
	// TODO It will become necessary to do build this on a device per device basis.
	std::ostringstream pipelineString;
	pipelineString << typeToSource(getType()) << " name=video_source device="
			<< getDevice()
			<< " ! capsfilter name=capsfilter caps=video/x-raw-rgb,width="
			<< preferredFormat.getWidth() << ",height="
			<< preferredFormat.getHeight() << ",framerate="
			<< preferredFormat.getPreferredFrameRate().getNumerator() << "/"
			<< preferredFormat.getPreferredFrameRate().getDenominator()
			<< ";video/x-raw-yuv,width=" << preferredFormat.getWidth()
			<< ",height=" << preferredFormat.getHeight() << ",framerate="
			<< preferredFormat.getPreferredFrameRate().getNumerator() << "/"
			<< preferredFormat.getPreferredFrameRate().getDenominator()
			<< " ! identity ! ffmpegcolorspace ";

	return pipelineString.str();
}

void GstVideoDevice::setGstPipeline(const std::string& pipeline) {
	gstreamerPipeline = pipeline;
}

}

