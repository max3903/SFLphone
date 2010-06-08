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

#ifndef __SFL_VIDEO_INPUT_SOURCE_GST_H__
#define __SFL_VIDEO_INPUT_SOURCE_GST_H__

#include "FrameFormat.h"
#include "GstVideoDevice.h"
#include "VideoInputSource.h"

#include <vector>
#include <string>
#include <stdexcept> 
#include <stdint.h>

#include <gst/gstelement.h>
#include <gst/app/gstappsink.h>

namespace sfl {

/**
 * This exception is thrown when a gstreamer exception occurs.
 */
class GstException: public std::runtime_error {
public:
	GstException(const std::string& msg) :
		std::runtime_error(msg) {
	}
};

/**
 * This exception is thrown when an expected plugin is missing from gstreamer.
 */
class MissingGstPluginException: public GstException {
public:
	MissingGstPluginException(const std::string& msg) :
		GstException(msg) {
	}
};


/**
 * This class captures video frames asynchronously via Gstreamer.
 */
class VideoInputSourceGst: public VideoInputSource {
public:
	VideoInputSourceGst();
	~VideoInputSourceGst();

	/**
	 * @Override
	 */
	std::vector<VideoDevicePtr> enumerateDevices(void);

	/**
	 * Shadowing phenomenon. Must be kept in order to hint the compiler.
	 */
	using VideoInputSource::open;

	/**
	 * @Override
	 */
	void open(VideoDevicePtr device) throw (VideoDeviceIOException);

	/**
	 * @Override
	 */
	void close() throw (VideoDeviceIOException);

	/**
	 * @Override
	 */
	void grabFrame() throw (VideoDeviceIOException);

	/**
	 * Check if we are capturing frames.
	 * @return true If the pipeline is running.
	 */
	bool isRunning() { return pipelineRunning; }

	static const char* APPSINK_NAME;
	static const int STATE_CHANGE_MAX_WAIT = 10;
private:
	/**
	 * Make sure that all the plugins are available in gstreamer.
	 * @param plugins A vector of strings, containing the names of the plugins.
	 */
	void ensurePluginAvailability(std::vector<std::string>& plugins)
			throw (MissingGstPluginException);
	/**
	 * Dispatch a frame event to the registered observers.
	 * This will pull the frame from the queue, and push it
	 * to each observer.
	 */
	void dispatchEvent();

	/**
	 * Callback for appsink in gstreamer.
	 * This is handled internally. When a new frame is available,
	 * this method is called, then dispath the event to the
	 * registered observers.
	 * @param sink The appsink.
	 * @param data A pointer on the source object.
	 */
	static GstFlowReturn onNewBuffer(GstAppSink * sink, gpointer data);

	/**
	 * Inspect the device and find its capabilities such as the framerate and supported resolutions.
	 * This code is adapted from Cheese, in cheese-webcam.c
	 * @param source The gst video source. Eg : v4l2source
	 * @param device The video device for that source. Eg : /dev/video0
	 * @return The frame formats supported for this device. The information contained in every of those objects will include
	 * the resolution, mimetype and framerate.
	 * @throws GstException if a gstreamer error occurs.
	 */
	std::vector<FrameFormat> getWebcamCapabilities(VideoSourceType type, const std::string& device) throw(GstException);

	/**
	 * This methods figures out the : mimitype, resolution, and frame rate informations.
	 * This code is adapted from Cheese, in cheese-webcam.c
	 * Submethod used in getWebcamCapabilities.
	 * @param caps The GstCaps for a given device.
	 */
	std::vector<FrameFormat> getSupportedFormats(GstCaps* caps);

	/**
	 * Find the supported framerates for a specified video format.
	 * This code is adapted from Cheese, in cheese-webcam.c
	 * Submethod used in getWebcamCapabilities.
	 * @param structure A GstStructure, as obtained in the implementation of getSupportedFormats.
	 * @return A vector containing all the supported framerates for the given device.
	 */
	std::vector<FrameRate> getSupportedFramerates(GstStructure* structure);

	std::vector<VideoDevice*> getXimageSource() // TODO Re-integrate
			throw (MissingGstPluginException);
	std::vector<VideoDevice*> getVideoTestSource() // TODO Re-integrate
			throw (MissingGstPluginException);

	std::vector<VideoDevicePtr> getV4l2Devices()
			throw (MissingGstPluginException);

	/**
	 * Functor to ease conversion from video type enum to string.
	 * @param type The enum type.
	 */
	VideoSourceTypeToGstSourceString videoTypeToString;

	GstElement * pipeline;
	bool pipelineRunning;

};
}
#endif
