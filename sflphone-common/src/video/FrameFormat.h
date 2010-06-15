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

#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <stdexcept>

#include "config.h"

#ifdef WORDS_BIGENDIAN
#define CSP_RGBA CSP_RGB32_1
#define CSP_BGRA CSP_BGR32_1
#define CSP_ARGB CSP_RGB32
#define CSP_ABGR CSP_BGR32
#else
#define CSP_RGBA CSP_BGR32
#define CSP_BGRA CSP_RGB32
#define CSP_ARGB CSP_BGR32_1
#define CSP_ABGR CSP_RGB32_1
#endif

/**
 * This enum corresponds exactly to those types found in FFMPEG.
 * The reason for providing this enum again here is to eliminate the coupling with the library
 * and allow child Decoder objects not to depend on it.
 *
 * CSP_RGB32 is handled in an endian-specific manner. A RGBA
 * color is put together as:
 *  (A << 24) | (R << 16) | (G << 8) | B
 * This is stored as BGRA on little endian CPU architectures and ARGB on
 * big endian CPUs.
 *
 * When the pixel format is palettized RGB (CSP_PAL8), the palettized
 * image data is stored in AVFrame.data[0]. The palette is transported in
 * AVFrame.data[1] and, is 1024 bytes long (256 4-byte entries) and is
 * formatted the same as in CSP_RGB32 described above (i.e., it is
 * also endian-specific). Note also that the individual RGB palette
 * components stored in AVFrame.data[1] should be in the range 0..255.
 * This is important as many custom PAL8 video codecs that were designed
 * to run on the IBM VGA graphics adapter use 6-bit palette components.
 */
enum PixelColorSpace {
    CSP_NONE= -1,
    CSP_YUV420P,   ///< Planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    CSP_YUYV422,   ///< Packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    CSP_RGB24,     ///< Packed RGB 8:8:8, 24bpp, RGBRGB...
    CSP_BGR24,     ///< Packed RGB 8:8:8, 24bpp, BGRBGR...
    CSP_YUV422P,   ///< Planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    CSP_YUV444P,   ///< Planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
    CSP_RGB32,     ///< Packed RGB 8:8:8, 32bpp, (msb)8A 8R 8G 8B(lsb), in cpu endianness
    CSP_YUV410P,   ///< Planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
    CSP_YUV411P,   ///< Planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
    CSP_RGB565,    ///< Packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), in cpu endianness
    CSP_RGB555,    ///< Packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), in cpu endianness most significant bit to 1
    CSP_GRAY8,     ///<        Y        ,  8bpp
    CSP_MONOWHITE, ///<        Y        ,  1bpp, 1 is white
    CSP_MONOBLACK, ///<        Y        ,  1bpp, 0 is black
    CSP_PAL8,      ///< 8 bit with CSP_RGB32 palette
    CSP_YUVJ420P,  ///< Planar YUV 4:2:0, 12bpp, full scale (jpeg)
    CSP_YUVJ422P,  ///< Planar YUV 4:2:2, 16bpp, full scale (jpeg)
    CSP_YUVJ444P,  ///< Planar YUV 4:4:4, 24bpp, full scale (jpeg)
    CSP_XVMC_MPEG2_MC,///< XVideo Motion Acceleration via common packet passing(xvmc_render.h)
    CSP_XVMC_MPEG2_IDCT,
    CSP_UYVY422,   ///< Packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
    CSP_UYYVYY411, ///< Packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
    CSP_BGR32,     ///< Packed RGB 8:8:8, 32bpp, (msb)8A 8B 8G 8R(lsb), in cpu endianness
    CSP_BGR565,    ///< Packed RGB 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), in cpu endianness
    CSP_BGR555,    ///< Packed RGB 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), in cpu endianness most significant bit to 1
    CSP_BGR8,      ///< Packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
    CSP_BGR4,      ///< Packed RGB 1:2:1,  4bpp, (msb)1B 2G 1R(lsb)
    CSP_BGR4_BYTE, ///< Packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
    CSP_RGB8,      ///< Packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
    CSP_RGB4,      ///< Packed RGB 1:2:1,  4bpp, (msb)2R 3G 3B(lsb)
    CSP_RGB4_BYTE, ///< Packed RGB 1:2:1,  8bpp, (msb)2R 3G 3B(lsb)
    CSP_NV12,      ///< Planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 for UV
    CSP_NV21,      ///< as above, but U and V bytes are swapped

    CSP_RGB32_1,   ///< Packed RGB 8:8:8, 32bpp, (msb)8R 8G 8B 8A(lsb), in cpu endianness
    CSP_BGR32_1,   ///< Packed RGB 8:8:8, 32bpp, (msb)8B 8G 8R 8A(lsb), in cpu endianness

    CSP_NB,        ///< number of pixel formats, DO NOT USE THIS if you want to link with shared libav* because the number of formats might differ between versions
};

namespace sfl {

/**
 * Predicate for telling if a given color space format is planar.
 */
struct IsFormatPlanar: std::unary_function<PixelColorSpace, bool> {
	bool operator ()(PixelColorSpace format) {
		switch(format) {
		case CSP_YUV420P:
		case CSP_YUV422P:
		case CSP_YUV444P:
		case CSP_YUV410P:
		case CSP_YUV411P:
		case CSP_YUVJ420P:
		case CSP_YUVJ422P:
		case CSP_YUVJ444P:
			return true;
		case CSP_NONE:
		case CSP_NB:
		default:
			return false;
		}
	}
};

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
 * A FrameFormat objects is used to hold various information about a video frame.
 */
class FrameFormat {
public:
	/**
	 * Default constructor, with default values. Warning: these values might
	 * not be supported by the device. Hence do not use this constructor unless
	 * you really know that it won't cause any trouble later.
	 */
	FrameFormat() throw (InvalidFrameRateException);

	/**
	 * @param mimetype The mimetype. http://www.gstreamer.net/data/doc/gstreamer/head/pwg/html/section-types-definitions.html#table-video-types
	 * @param width The frame width.
	 * @param height The frame height.
	 * @param framerates A set containing all the supported framerates for that format.
	 */
	FrameFormat(const std::string& mimetype, int width, int height, std::set<
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
	 * @return The color space that was defined.
	 */
	PixelColorSpace getColorSpace() const;

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
	 * @param colorSpace The color space of use.
	 * @see PixelFormat
	 */
	void setColorSpace(PixelColorSpace colorSpace);

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
	inline bool operator==(const FrameFormat& other) const {
		if ((getWidth() == other.getWidth()) && (getHeight()
				== other.getHeight())) {
			return true;
		}

		return false;
	}

	static const char* DEFAULT_MIMETYPE;
	static const int DEFAULT_WIDTH = 320;
	static const int DEFAULT_HEIGHT = 240;
	static const PixelColorSpace DEFAULT_COLORSPACE = CSP_RGBA;
	static const FrameRate DEFAULT_FRAMERATE;

private:
	void init(const std::string& mimetype, int width, int height, std::set<
			FrameRate>& framerates) throw (InvalidFrameRateException);

	std::string mimetype;
	std::set<FrameRate> framerates;
	FrameRate preferredFramerate;
	int width;
	int height;
	PixelColorSpace fourcc;
};

}
#endif
