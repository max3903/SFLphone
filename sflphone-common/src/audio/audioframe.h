/*
 *  Copyright (C) 2010 Savoir-Faire Linux inc.
 *  Author: Alexandre Savard <alexandre.savard@savoirfairelinux.com>
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

#ifndef __AUDIO_FRAME__
#define __AUDIO_FRAME__


class AudioFrame 
{
    public:
        /**
         * Constructor
         * @param size Size in number of samples
	 * @param nbChannle Number of channels for this frame
	 * @param original sampling rate
         */
        AudioFrame (const unsigned int size, const unsigned int nbChannel, const unsigned int smplRate);

	/**
         * Constructor
         * @param size Size in number of samples
         * @param nbChannle Number of channels for this frame
         * @param original sampling rate
	 * @param buffer, data to be copied in this frame
         */
        AudioFrame (const unsigned int size, const unsigned int nbChannel, const unsigned int smplRate, SFLDataFormat *buffer);

        /**
         * Destructor
         */
        ~AudioFrame();

	/**
	 * Return the number of sanmples contained by this frame
	 */
	unsigned int getSize(void);

	/**
	 * Return the number of channels contained by this frame
	 */
	unsigned int getNbChannel(void);

	/**
	 * Return the original sampling rate for data at creation of this frame
	 */
	unsigned int getSmplRate(void);

	/**
	 * Copy data contained by this frame into the provided buffer
	 * buffer should be of the same size of the data contained by this frame
	 */
	unsigned int  getData(SFLDataFormat *buffer, unsigned int channel);

	/**
	 * Copy data into the frame. Data should be of the same size of the frame
	 */
	unsigned int putData(SF:DataFormat *buffer, unsigned int channel);

    private:

	/**
	 * Number of samples contained by this frame
	 */
	unsigned int _size

	/**
	 * Number of channels (1 mono, 2 stereo)
	 */
	unsigned int _nbChannel;

	/**
	 * Original sampling rate at the creation of this frame
	 */
	unsigned int _smplRate;

	/**
	 * Buffer to store data for this frame
	 */
	SFLDataFormat *internalBuffer;
};


#endif 
