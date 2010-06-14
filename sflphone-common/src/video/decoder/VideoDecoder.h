/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */
 
#ifndef __SFL_DECODER_H__
#define __SFL_DECODER_H__

#include "video/VideoFrame.h"
#include "video/FrameFormat.h"

namespace sfl
{
	/**
	 * Abstract base class for every video encoder.
	 */
	class VideoDecoder
	{
		public:
			/**
			 * @param The desired output frame format.
			 */
			VideoDecoder(const FrameFormat& format);

			/**
			 * @param frame The video frame to encode.
			 * @param size The frame size.
			 */
			virtual void decode(const uint8_t* frame, size_t size) = 0;

			/**
			 * @return A pointer to a new copy of the current object.
			 */
			virtual VideoDecoder* clone();

			/**
			 * @return The Frame format defined for the output image.
			 */
			const FrameFormat& getFrameFormat() const;

		private:
			FrameFormat format;
	};

}

#endif /*DECODER_H_*/
