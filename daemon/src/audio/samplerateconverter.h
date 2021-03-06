/*
 *  Copyright (C) 2004, 2005, 2006, 2008, 2009, 2010, 2011 Savoir-Faire Linux Inc.
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Alexandre Savard <alexandre.savard@savoirfairelinux.com>
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

#ifndef _SAMPLE_RATE_H
#define _SAMPLE_RATE_H

#include <samplerate.h>
#include <math.h>

#include "global.h"


class SamplerateConverter
{
    public:
        /**
         * Samplerate converter is used for several situation:
        * streaming conversion (RTP, IAX), audiolayer conversion,
        * audio files conversion. Parameters are used to compute
        * internal buffer size. Converter must be reinitialized
        * every time these parameters change
        */
        SamplerateConverter (int freq);

        /** Destructor */
        ~SamplerateConverter (void);

        /**
         * resample from the samplerate1 to the samplerate2
         * @param data  The data buffer
         * @param SamplerateConverter1 The original sample rate
         * @param SamplerateConverter2 The desired sample rate
         * @param nbSamples	  The number of samples to process
         */
        void resample (SFLDataFormat* dataIn , SFLDataFormat* dataOut , int samplerate1 , int samplerate2 , int nbSamples);

        /**
         * Convert short table to floats for audio processing
         * @param in the input (short) array
         * @param out The resulting (float) array
         * @param len The number of elements in both tables
         */
        void Short2FloatArray (const short *in, float *out, int len);


    private:
        // Copy Constructor
        SamplerateConverter (const SamplerateConverter& rh);

        // Assignment Operator
        SamplerateConverter& operator= (const SamplerateConverter& rh);

        /* temporary buffers */
        float32* _floatBufferIn;
        float32* _floatBufferOut;
        size_t _samples; // size in samples of temporary buffers
        int _maxFreq; // maximal output frequency

        SRC_STATE* _src_state;
};

#endif //_SAMPLE_RATE_H

