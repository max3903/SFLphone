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
#ifndef __SFL_FRAME_FORMAT_H__
#define __SFL_FRAME_FORMAT_H__

#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>

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
 * A FrameFormat objects represents a supported resolution for a video frame.
 * Expressed in the number of horizontal and vertical pixels.
 */
class FrameFormat {
public:
	/**
	 * Default constructor, with default values. Warning: these values might
	 * not be supported by the device. Hence do not use this constructor unless
	 * you really know that this won't cause any trouble.
	 */
	FrameFormat() throw (InvalidFrameRateException);

	/**
	 * @param mimetype The mimetype.
	 * @param width The frame width.
	 * @param height The frame height.
	 * @param framerates A vector containing all the supported framerates for that format.
	 */
	FrameFormat(const std::string& mimetype, int width, int height,
			std::vector<FrameRate> framerates)
			throw (InvalidFrameRateException);

	/**
	 * @return The mimetype that corresponds to this video format. Eg: video/x-raw-rgb
	 */
	std::string getMimetype() const {
		return mimetype;
	}

	/**
	 * @return The frame's width, in pixels.
	 */
	int getWidth() const {
		return width;
	}

	/**
	 * @return The frame's height, in pixels.
	 */
	int getHeight() const {
		return height;
	}

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
	 * A given video format may have multiple associated framerates.
	 * @param numerator
	 * @param denominator
	 */
	void addFramerate(int numerator, int denominator);

	/**
	 * @return The supported framerates for this format.
	 */
	std::vector<FrameRate> getFrameRates() {
		return framerates;
	}

	/**
	 * @param framerate A valid framerate to be used as the preferred framerate.
	 * @throws InvalidFrameRateException If the framerate that was passed is incompatible. The framerate must be chosen among
	 * the list returned by getFrameRates().
	 */
	void setPreferredFrameRate(const FrameRate& framerate)
			throw (InvalidFrameRateException);

	/**
	 * @return the preferred framerate as set by the user, or the fastest one if none is specified.
	 */
	FrameRate getPreferredFrameRate();

	/**
	 * This interpretation of equality is needed in finding duplicates.
	 */
	inline bool operator==(const FrameFormat& other) const {
		if ((getWidth() == other.getWidth()) && (getHeight() == other.getHeight())) {
			return true;
		}

		return false;
	}

	static const char* DEFAULT_MIMETYPE;
	static const int DEFAULT_WIDTH = 320;
	static const int DEFAULT_HEIGHT = 240;
	static const FrameRate DEFAULT_FRAMERATE;
private:
	void init(const std::string& mimetype, int width, int height, std::vector<
			FrameRate>& framerates) throw (InvalidFrameRateException);
	std::string mimetype;
	std::vector<FrameRate> framerates;
	FrameRate preferredFramerate;
	int width;
	int height;
};

}
#endif
