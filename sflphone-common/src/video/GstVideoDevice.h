/*
 *  Copyright (C) 2010 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __SFL_GST_VIDEO_DEVICE_H__
#define __SFL_GST_VIDEO_DEVICE_H__

#include "VideoDevice.h"
#include "FrameFormat.h"

#include <vector>
#include <string>
#include <functional>
#include "../logger.h"

namespace sfl {

/**
 * Function object for converting a VideoSourceType to the corresponding Gstreamer source.
 * For example : V4L2 is provided by v4l2src in Gstreamer.
 */
struct VideoSourceTypeToGstSourceString: public std::unary_function<
		std::string, VideoSourceType> {
	std::string operator()(VideoSourceType type) {
		switch (type) {
		case V4L:
			return std::string("v4lsrc");
		case V4L2:
			return std::string("v4l2src");
		case DV1394:
			return std::string("dv1394src");
		case XIMAGE:
			return std::string("ximagesrc");
		case TEST:
			return std::string("videotestsrc");
		case IMAGE:
		case NONE:
		default:
			return std::string("undefined");
		}
	}
};

/**
 * Specialisation of the VideoDevice type to hold additional information needed by Gstreamer.
 */
class GstVideoDevice: public VideoDevice {
public:
	/**
	 * Builds a GST pipeline automatically.
	 *
	 * @param type One of the available source type (eg. V4L2)
	 * @param formats The video formats that this device supports.
	 * @param device The device identifier (eg: /dev/video0).
	 * @param name The representative, and unique name of this device.
	 */
	GstVideoDevice(VideoSourceType type, std::vector<VideoFormat> formats,
			const std::string& device, const std::string& name);

	/**
	 * Copy constructor.
	 * @param other The object to copy from.
	 */
	GstVideoDevice(const GstVideoDevice& other);

	~GstVideoDevice() {}

	/**
	 * @return The string representation of the Gstreamer pipeline needed to access this device.
	 */
	std::string getGstPipeline() const;

	/**
	 * @param pipeline A string that represents the GST graph.
	 */
	void setGstPipeline(const std::string& pipeline);

	/**
	 * Build and set the gst pipeline corresponding to the specified VideoFormat.
	 * @precondition A preferred frame format must have been set.
	 */
	void setGstPipelineFromFormat();

	/**
	 * @Override
	 * @postcondition The preferred format will be set and the corresponding GstPipeline will be generated.
	 */
	void setPreferredFormat(const VideoFormat& format);

private:
	std::string gstreamerPipeline;
	VideoSourceTypeToGstSourceString typeToSource;
};

/**
 * Video devices are returned as std::shared_ptr in std::vector.
 */
typedef std::shared_ptr<GstVideoDevice> GstVideoDevicePtr;
}
#endif
