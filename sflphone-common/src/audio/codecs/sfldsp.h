/*
 * sfldsp.h
 *	Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Created on: 2010-08-05
 * Author:  Jeremy Palvadeau <jeremy.palvadeau@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

#ifndef SFLDSP_H_
#define SFLDSP_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    /*
     *  @func   sfl_dsp_init
     *
     *  @desc   This function allocates and initializes resources used by
     *          this application.
     *
     *  @arg    dspExecutable
     *              DSP executable name.
     *  @arg    bufferSize
     *              String representation of buffer size to be used
     *              for data transfer.
     *  @arg    strNumIterations
     *              Number of iterations a data buffer is transferred between
     *              GPP and DSP in string format.
     *  @arg    processorId
     *              ID of the DSP processor
     *  @arg    factor
     *              factor to multiply buffer
     */
    void sfl_dsp_init (char * dspExecutable, char * strBufferSize, char * strNumIterations, char * strProcessorId, char * strScaleFactor);

    /*
     *  @func   sfl_dsp_process
     *
     *  @desc   Process buffer.
     *
     *  @arg    buffer_in
     *
     *  @arg    buffer_out
     *
     *  @arg    nbelem
     *              number of object
     */
    void sfl_dsp_process (short *buffer_in, short *buffer_out, unsigned int nbelem);

    /*
     *  @func   sfl_dsp_deinit
     *
     *  @desc   This function releases resources allocated earlier by call to
     *          sfl_dsp_init ().
     *          During cleanup, the allocated resources are being freed
     *          unconditionally. Actual applications may require stricter check
     *          against return values for robustness.
     *
     *  @arg    processorId
     *              ID of the DSP processor
     */
    void sfl_dsp_deinit (unsigned char processorId);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* sfldsp_H_ */

