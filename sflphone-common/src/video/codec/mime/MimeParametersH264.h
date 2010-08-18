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

#ifndef __SFL_MIME_PARAMETERS_H264_H__
#define __SFL_MIME_PARAMETERS_H264_H__

#include "MimeParameters.h"

namespace sfl
{
/**
 *
 * MIME media type name: video
 *
 * MIME subtype: H264
 *
 * fmtp:96 profile-level-id=42801E; packetization-mode=1; sprop-parameter-sets=J0KAFJWgUH5A,KM4CvIA=
 */
MIME_PAYLOAD_FORMAT_DEFINITION("video", H264, 96, 90000)
	/**
	 *	A base16 [6] (hexadecimal) representation of the following three bytes in the sequence
	 *	parameter set NAL unit specified in [1]: 1) profile_idc, 2) a byte herein referred to as
	 *	profile-iop, composed of the values of constraint_set0_flag, constraint_set1_flag,
	 *	constraint_set2_flag, and reserved_zero_5bits in bit-significance order, starting from the
	 *	most significant bit, and 3) level_idc.  Note that reserved_zero_5bits is required to be
	 *	equal to 0 in [1], but other values for it may be specified in the future by ITU-T or ISO/IEC.
	 */
	MIME_PARAMETER("profile-level-id");

	/**
	 * This parameter signals the properties of an
	 * RTP payload type or the capabilities of a
	 * receiver implementation.
	 */
	MIME_PARAMETER ("packetization-mode");

	/**
	 * This parameter MAY be used to convey
	 * any sequence and picture parameter set NAL
	 * units (herein referred to as the initial
	 * parameter set NAL units) that MUST precede any
	 * other NAL units in decoding order.
	 */
	MIME_PARAMETER("sprop-parameter-sets");

	/**
	 *  The value of max-mbps is an integer indicating the maximum macroblock processing rate in units
	 *   of macroblocks per second.
	 */
	MIME_PARAMETER("max-mbps");

	/**
	 * The value of max-fs is an integer indicating
	 * the maximum frame size in units of macroblocks.
	 */
	MIME_PARAMETER("max-fs");

	/**
	 *  The value of max-cpb is an integer indicating
	 *	the maximum coded picture buffer size in units
	 *	of 1000 bits for the VCL HRD parameters (see
	 *	A.3.1 item i of [1]) and in units of 1200 bits
	 *	for the NAL HRD parameters (see A.3.1 item j of
	 *	[1]).
	 */
	MIME_PARAMETER("max-cpb");

	/**
	 * The value of max-dpb is an integer indicating
	 * the maximum decoded picture buffer size in units of 1024 bytes.
	 */
	MIME_PARAMETER("max-dpb");

	/**
	 * The value of max-br is an integer indicating
	 * the maximum video bit rate in units of 1000
	 * bits per second for the VCL HRD parameters (see
	 * A.3.1 item i of [1]) and in units of 1200 bits
	 * per second for the NAL HRD parameters (see A.3.1 item j of [1]).
	 */
	MIME_PARAMETER("max-br");

	/**
	 * This parameter signals the capabilities of a receiver implementation.
	 * When equal to 0, the
	 * parameter indicates that the receiver makes no
	 * attempt to use redundant coded pictures to
	 * correct incorrectly decoded primary coded
	 * pictures.  When equal to 0, the receiver is not
	 * capable of using redundant slices; therefore, a
	 * sender SHOULD avoid sending redundant slices to
	 * save bandwidth.
	 */
	MIME_PARAMETER("redundant-pic-cap");

	/**
	 * This parameter MAY be used to signal whether
	 * the receiver of this parameter is allowed to
	 * add parameter sets in its signaling response
	 * using the sprop-parameter-sets MIME parameter.
	 */
	MIME_PARAMETER ("parameter-add");

	/**
	 * This parameter MUST NOT be present
	 * when packetization-mode is not present or the
	 * value of packetization-mode is equal to 0 or 1.
	 * This parameter MUST be present when the value
	 * of packetization-mode is equal to 2.
	 */
	MIME_PARAMETER ("sprop-interleaving-depth");

	/**
	 * This parameter MUST NOT be present when
	 * packetization-mode is not present or the value
	 * of packetization-mode is equal to 0 or 1.  It
	 * MUST be present when the value of
	 * packetization-mode is equal to 2.
	 *
	 */
	MIME_PARAMETER ("sprop-deint-buf-req");

	/**
	 * This parameter signals the capabilities of a
	 * receiver implementation and indicates the
	 * amount of deinterleaving buffer space in units
	 * of bytes that the receiver has available for
	 * reconstructing the NAL unit decoding order.
	 */
	MIME_PARAMETER ("deint-buf-cap");

	/**
	 * This parameter MAY be used to signal the
	 * properties of a NAL unit stream.  The parameter
	 * MUST NOT be present, if the value of
	 * packetization-mode is equal to 0 or 1.
	 */
	MIME_PARAMETER ("sprop-init-buf-time");

	/**
	 * This parameter MAY be used to signal the
	 * properties of a NAL unit stream.  It MUST NOT
	 * be used to signal transmitter or receiver or
	 * codec capabilities.  The parameter MUST NOT be
	 * present if the value of packetization-mode is
	 * equal to 0 or 1.  sprop-max-don-diff is an
	 * integer in the range of 0 to 32767, inclusive.
	 */
	MIME_PARAMETER ("sprop-max-don-diff");

	/**
	 * This parameter MAY be used to signal the
	 * capabilities of a receiver.  The parameter MUST
	 * NOT be used for any other purposes.  The value
	 * of the parameter indicates the largest NALU
	 * size in bytes that the receiver can handle
	 * efficiently.
	 */
	MIME_PARAMETER ("max-rcmd-nalu-size");
MIME_PAYLOAD_FORMAT_DEFINITION_END()

}
#endif
