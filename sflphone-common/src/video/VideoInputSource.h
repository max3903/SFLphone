/*
 *  Copyright (C) 2006-2010 Savoir-Faire Linux inc.
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
#ifndef __SFL_VIDEO_INPUT_SOURCE_H___
#define __SFL_VIDEO_INPUT_SOURCE_H___

#include "VideoDevice.h"

#include <cc++/exception.h>

#include <vector> 
#include <string>
#include <stdexcept>

#include <stdint.h>

namespace ost {
class Mutex;
}

namespace sfl {

class VideoFrame;

/**
 * This exception is thrown when an IO operation fails for a given video device.
 */
class VideoDeviceIOException: public std::runtime_error {
public:
	VideoDeviceIOException(const std::string& msg) :
		std::runtime_error(msg) {
	}
};

/**
 * This exception is thrown when the specified video device could not be found.
 */
class UnknownVideoDeviceException: public std::runtime_error {
public:
	UnknownVideoDeviceException(const std::string& msg) :
		std::runtime_error(msg) {
	}
};

/**
 * This exception is thrown when no video device is available when attempting an operation.
 */
class NoVideoDeviceAvailableException: public std::runtime_error {
public:
	NoVideoDeviceAvailableException(const std::string& msg) :
		std::runtime_error(msg) {
	}
};

/**
 * Interface for those objects which want
 * to be notified upon the arrival of new
 * frames for a specified video input source.
 */
class VideoFrameObserver {
public:
	/**
	 * @param frame The new frame in whichever format it was requested.
	 * @param size The size of the frame.
	 */
	virtual void onNewFrame(const VideoFrame* frame) = 0;
};

/**
 * Base abstract class for asynchronous or synchronous access to a video capture device.
 */
class VideoInputSource {
public:
	/**
	 * @return a vector containing the name of all the video devices available from this source.
	 */
	virtual std::vector<VideoDevicePtr> enumerateDevices(void) = 0;

	/**
	 * Open the specified video device. If no device was specified in prior calls, this function
	 * will attempt to handle it by picking up the first available device.
	 * @throws VideoDeviceIOException if case a general IO error occurs.
	 * @throws NoVideoDeviceAvailableException if no video device can be found.
	 * @see VideoInputSource#setDevice
	 * @see VideoInputSource#open(VideoDevice device)
	 */
	void open() throw (VideoDeviceIOException, NoVideoDeviceAvailableException);

	/**
	 * Set and open a video device.
	 * @param device The video device to set and open.
	 * @postcondition The video stream will be opened, and the current device will be set to the specified one.
	 * @see VideoInputSource#open()
	 */
	virtual void open(VideoDevicePtr device) throw (VideoDeviceIOException) = 0;

	/**
	 * Close the currently opened device.
	 * @precondition The device should have been opened prior to that call.
	 */
	virtual void close() throw (VideoDeviceIOException) = 0;

	/**
	 * Grab a frame from the specified capture device.
	 * @precondition The device should have been opened prior to that call.
	 * @postcondition getCurrentFrame() will return the frame that was captured.
	 */
	virtual void grabFrame() throw (VideoDeviceIOException) = 0;

	/**
	 * Set the device to use to the first available one.
	 * @see VideoInputSource#enumerateDevices()
	 * @throws NoVideoDeviceAvailableException
	 */
	void setDevice();

	/**
	 * @param device The device to use.
	 */
	void setDevice(VideoDevicePtr device);

	/**
	 * Try to find a device name with the provided name, and set it as the source device.
	 * @param device The descriptive name for this device.
	 */
	void setDevice(const std::string& device)
			throw (UnknownVideoDeviceException);

	/**
	 * @return the current device that is being used.
	 */
	inline VideoDevicePtr getDevice() {
		return currentDevice;
	}

	/**
	 * The user can choose to specify different height than what the device is actually offering.
	 * @return the current scaled height.
	 */
	inline int getScaledHeight() {
		return scaledHeight;
	}

	/**
	 * @return the current scaled width.
	 */
	inline int getScaledWidth() {
		return scaledWidth;
	}

	/**
	 * @return the current depth after reformatting.
	 */
	inline int getReformattedDepth() {
		return reformattedDepth;
	}

	/**
	 * @param width The desired width after scaling.
	 */
	inline void setScaledWidth(int width) {
		scaledWidth = width;
	}

	/**
	 * @param height The desired width after scaling.
	 */
	inline void setScaledHeight(int height) {
		scaledHeight = height;
	}

	/**
	 * @param depth The desired width after scaling.
	 */
 	inline void setReformattedDepth(int depth) {
 		reformattedDepth = depth;
 	}

	/**
	 * Register a new video frame observer.
	 * The observer will get called when a frame becomes available.
	 * @param observer The video frame observer.
	 */
	void addVideoFrameObserver(VideoFrameObserver* observer);

	/**
	 * Remove a frame observe.
	 * @param observer The video frame observer.
	 * @postcondition The observer won't be notified of further changes.
	 */
	void removeVideoFrameObserver(VideoFrameObserver* observer);

	/**
	 * @return The current frame.
	 */
	VideoFrame* getCurrentFrame();

	/**
	 * Destructor.
	 */
	~VideoInputSource();

	/**
	 * Constructor. The user must then enumerateDevices() and set the device to use.
	 * @see sfl::VideoInputSource#enumerateDevices()
	 */
	VideoInputSource();

	static const char* CLIENT_MIMETYPE;
	static const int CLIENT_BPP = 32;
	static const int CLIENT_DEPTH = 32;
protected:
	/**
	 * Call every observers with the current frame as an argument.
	 */
	void notifyAllFrameObserver();

	/**
	 * @param frame The current frame.
	 * @param size The buffer size.
	 */
	void setCurrentFrame(const uint8_t* frame, size_t size);

	VideoDevicePtr currentDevice;
private:
	ost::Mutex* frameMutex;
	VideoFrame* currentFrame;
	std::vector<VideoFrameObserver*> videoFrameObservers;

	int scaledWidth;
	int scaledHeight;
	int reformattedDepth;
};

}
#endif /*VIDEOINPUTSOURCE_H_*/
