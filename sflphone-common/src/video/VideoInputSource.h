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
#ifndef VIDEOINPUTSOURCE_H_
#define VIDEOINPUTSOURCE_H_

#include <vector> 
#include <string>
#include <ostream>
#include <memory>
#include <stdexcept>
#include <stdint.h>

#include <cc++/thread.h>
#include <cc++/exception.h>

#include <gst/gst.h>

namespace sfl {

/**
 * Video source types that might be supported.
 */
enum VideoSourceType {
	V4L, V4L2, DV1394, XIMAGE, IMAGE, TEST, NONE
};

/**
 * This exception is thrown when an IO operation fails for a given video device.
 */
class VideoDeviceIOException: public ost::IOException {
public:
	VideoDeviceIOException(const std::string& msg) :
		ost::IOException(msg) {
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
 * Representation of a given gstreamer video device.
 */
class VideoDevice {
public:
	/**
	 * @param type One of the available source type (eg. V4L2)
	 * @param name The representative, and unique name of this device.
	 * @param description A description, additional info.
	 */
	VideoDevice(VideoSourceType type, std::string name, std::string description) {
		this->type = type;
		this->name = name;
		this->description = description;
	}

	/**
	 * @return The video source type (eg. V4L2)
	 */
	VideoSourceType getType() {
		return type;
	}
	/**
	 * @return The representative, and unique name of this device.
	 */
	std::string getName() {
		return name;
	}
	/**
	 * @return A description, additional info.
	 */
	std::string getDescription() {
		return description;
	}

	/**
	 * The string representation for this device.
	 */
	std::string toString() {
		return name;
	}

private:
	VideoSourceType type;
	std::string name;
	std::string description;
};

/**
 * This class represents a captured frame.
 */
class VideoFrame {
public:
	/**
	 * @param frame The frame data.
	 * @param size The frame size.
	 * @param height The frame height.
	 * @param width The frame width.
	 */
	VideoFrame(const uint8_t* frame, const size_t size, unsigned int depth, unsigned int height,
			unsigned int width) {

		std::cout << "Creating new frame of size ";
		std::cout << size << std::endl;

		this->frame = (uint8_t*) malloc(size);
		memcpy(this->frame, frame, size);

		this->size = size;
		this->height = height;
		this->width = width;
		this->depth = depth;
	}

	~VideoFrame()
	{
		free(frame);
	}

	/**
	 * @return The frame data.
	 */
	const uint8_t *getFrame() const {
		return frame;
	}
	/**
	 * @return The frame height.
	 */
	unsigned int getHeight() const {
		return height;
	}

	/**
	 * @return The frame width.
	 */
	unsigned int getWidth() const {
		return width;
	}

	/**
	 * @return The frame depth in bytes (eg: 3 bytes)
	 */
	unsigned int getDepth() const {
		return depth;
	}

	/**
	 * @return The buffer size.
	 */
	size_t getSize() const {
		return size;
	}

private:
	uint8_t* frame;
	size_t size;
	unsigned int height;
	unsigned int width;
	unsigned int depth;
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
	virtual std::vector<VideoDevice*> enumerateDevices(void) = 0;

	/**
	 * Open the specified video device. Frame grabbing will be started on request, either via start(),
	 * or getFrame() (in the synchronous case).
	 * @param widht The desired width.
	 * @param height The desired height.
	 * @param fps The desired frame rate.
	 */
	virtual void open(int width, int height, int fps)
			throw (VideoDeviceIOException) = 0;

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
	 * @param device The device to use.
	 */
	inline void setDevice(VideoDevice* device) {
		currentDevice = device;
	}

	/**
	 * Try to find a device name with the provided name, and set it as the source device.
	 * @param device The descriptive name for this device.
	 */
	void setDevice(const std::string& device) throw(UnknownVideoDeviceException);

	/**
	 * @return the current device that is being used.
	 */
	inline VideoDevice* getDevice() {
		return currentDevice;
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
	 * @return The current width.
	 */
	unsigned int getWidth() { return this->width; }

	/**
	 * @return The current height.
	 */
	unsigned int getHeight() { return this->height; }

	/**
	 * @return The current depth.
	 */
	unsigned int getDepth() { return this->depth; }

	/**
	 * Destructor.
	 */
	virtual ~VideoInputSource();

	VideoInputSource();

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

	/**
	 * @param width The current width.
	 */
	void setWidth(unsigned int width) { this->width = width; }

	/**
	 * @param height The current height.
	 */
	void setHeight(unsigned int height) { this->height = height; }

	/**
	 * @param depth The current depth.
	 */
	void setDepth(unsigned int depth) { this->depth = depth; }

private:
	ost::Mutex frameMutex;
	VideoFrame* currentFrame;
	VideoDevice* currentDevice;
	std::vector<VideoFrameObserver*> videoFrameObservers;
	unsigned int width;
	unsigned int height;
	unsigned int depth;
};

}
#endif /*VIDEOINPUTSOURCE_H_*/
