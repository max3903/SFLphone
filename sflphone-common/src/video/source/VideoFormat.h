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
#ifndef __SFL_VIDEO_FORMAT_H__
#define __SFL_VIDEO_FORMAT_H__

#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <stdexcept>

namespace sfl {

/**
 * This exception is thrown when a framerate is not supported by the device.
 */
class InvalidFrameRateException: public std::runtime_error {
public:
	InvalidFrameRateException(const std::string& msg) :
		std::runtime_error(msg) {
	}
};

/**
 * An object that represents a frame rate, expressed as ratio.
 */
class FrameRate {
public:
	FrameRate() :
		numerator(0), denominator(1) {
	}
	;
	FrameRate(int numerator, int denominator) :
		numerator(numerator), denominator(denominator) {
	}
	int getNumerator() const {
		return numerator;
	}
	int getDenominator() const {
		return denominator;
	}
	inline bool operator==(const FrameRate& framerate) const {
		if ((getNumerator() == framerate.getNumerator()) && (getDenominator()
				== framerate.getDenominator())) {
			return true;
		}

		return false;
	}

	inline bool operator<(const FrameRate& other) const {
		return (getNumerator() / getDenominator()) < (other.getNumerator()
				/ other.getDenominator());
	}

	std::string toString() const {
		std::ostringstream objectString;

		objectString << numerator << "/" << denominator;

		return objectString.str();
	}
protected:
	int numerator;
	int denominator;
};

/**
 * A VideoFormat objects is used to hold various information about a video frame.
 */
class VideoFormat {
public:
	/**
	 * Default constructor, with default values. Warning: these values might
	 * not be supported by the device. Hence do not use this constructor unless
	 * you really know that it won't cause any trouble later.
	 */
	VideoFormat() throw (InvalidFrameRateException);

	/**
	 * @param mimetype The mimetype. http://www.gstreamer.net/data/doc/gstreamer/head/pwg/html/section-types-definitions.html#table-video-types
	 * @param width The frame width.
	 * @param height The frame height.
	 * @param framerate A supported framerate.
	 */
	VideoFormat(const std::string& mimetype, int width, int height, FrameRate framerate) throw (InvalidFrameRateException);

	/**
	 * @param mimetype The mimetype. http://www.gstreamer.net/data/doc/gstreamer/head/pwg/html/section-types-definitions.html#table-video-types
	 * @param width The frame width.
	 * @param height The frame height.
	 * @param framerates A set containing all the supported framerates for that format.
	 */
	VideoFormat(const std::string& mimetype, int width, int height, std::set<
			FrameRate> framerates) throw (InvalidFrameRateException);

	/**
	 * @return The mimetype that corresponds to this video format. Eg: video/x-raw-rgb
	 */
	std::string getMimetype() const;

	/**
	 * @return The frame's width, in pixels.
	 */
	int getWidth() const;

	/**
	 * @return The frame's height, in pixels.
	 */
	int getHeight() const;

	/**
	 * @return The supported framerates for this format.
	 */
	std::set<FrameRate> getFrameRates() const;

	/**
	 * @return the preferred framerate as set by the user, or the fastest one if none is specified.
	 */
	FrameRate getPreferredFrameRate() const;

	/**
	 * @return The FOURCC code corresponding to the image format in use.
	 */
	std::string getFourcc() const;

	/**
	 * @param mime The mimetype. Eg: video/x-raw-yuv
	 */
	void setMimetype(const std::string& mime);

	/**
	 * @param width The desired (and supported) width.
	 */
	void setWidth(const int& width);

	/**
	 * @param height The desired (and supported) height.
	 */
	void setHeight(const int& height);

	/**
	 * @param numerator The desired (and supported) numerator.
	 * @param denominator The desired (and supported) denominator.
	 */
	void setFramerate(const int& numerator, const int& denominator);

	/**
	 * @param framerate A string of the form "num/denom".
	 */
	void setFramerate(const std::string& framerate);

	/**
	 * @param framerate A valid framerate to be used as the preferred framerate.
	 * @throws InvalidFrameRateException If the framerate that was passed is incompatible. The framerate must be chosen among
	 * the list returned by getFrameRates().
	 */
	void setPreferredFrameRate(const FrameRate& framerate)
			throw (InvalidFrameRateException);

	/**
	 * @param fourcc The FOURCC code corresponding to the image format to use.
	 * @precondition The FOURCC code must be consistent with the MIME type.
	 */
	void setFourcc(const std::string& fourcc);

	/**
	 * A given video format may have multiple associated framerates.
	 * @param numerator
	 * @param denominator
	 */
	void addFramerate(int numerator, int denominator);

	/**
	 * @return A textual representation of this frame format.
	 */
	std::string toString() const {
		std::ostringstream objectString;

		objectString << mimetype << " " << width << "x" << height << " @ "
				<< preferredFramerate.getNumerator() << "/"
				<< preferredFramerate.getDenominator() << " fps";

		return objectString.str();
	}

	/**
	 * This interpretation of equality is needed in finding duplicates.
	 */
	inline bool operator==(const VideoFormat& other) const {
		if ((getWidth() == other.getWidth()) && (getHeight()
				== other.getHeight())) {
			return true;
		}

		return false;
	}

	/**
	 * Set to video/x-raw-rgb
	 */
	static const std::string DEFAULT_MIMETYPE;
	/**
	 * Set to ARGB
	 */
	static const std::string DEFAULT_FOURCC;
	/**
	 * Set to 30/1
	 */
	static const FrameRate DEFAULT_FRAMERATE;
	/**
	 * Set to 320
	 */
	static const int DEFAULT_WIDTH;
	/**
	 * Set to 240
	 */
	static const int DEFAULT_HEIGHT;

private:
	/**
	 * Helper method for constructors.
	 */
	void init(const std::string& mimetype, const std::string& fourcc, int width, int height,
			std::set<FrameRate>& framerates) throw (InvalidFrameRateException);

	std::string mimetype;
	std::string fourcc;
	std::set<FrameRate> framerates;
	FrameRate preferredFramerate;
	int width;
	int height;
};

}
#endif
