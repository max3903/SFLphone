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

#include "H264Depayloader.h"
#include <ccrtp/rtp.h>
#include <stdint.h>

namespace sfl {
H264Depayloader::H264Depayloader(const std::string sdp) {
}

H264Depayloader::~H264Depayloader() {
	assemblingBuffer.dispose();
}

/**
 * @Override
 */
void H264Depayloader::parseSdp(const std::string& sdp) {
	/**
	 * 	 TODO do the actual parsing. See section 8.1
	 *
	 * 	 - profile-level-id
	 *   - sprop-parameter-sets
	 *   - packetization-mode
	 *   - sprop-interleaving-depth
	 *   - sprop-deint-buf-req
	 *   - sprop-max-don-diff
	 *   - sprop-init-buf-time
	 */

	sdpUnparsed = sdp;
}

void H264Depayloader::handleFragmentationUnit(const ost::AppDataUnit* adu) {

	/**
	 * The FU header.
	 * +---------------+
	 * |0|1|2|3|4|5|6|7|
	 * +-+-+-+-+-+-+-+-+
	 * |S|E|R|  Type   |
	 * +---------------+
	 */
	const uint8* payload = adu->getData();
	size_t payloadSize = adu->getSize();

	bool startBit = ((payload[1] & 0x80) == 0x80) ? true : false;
	bool endBit = ((payload[1] & 0x40) == 0x40) ? true : false;

	// Start of a fragmented NAL unit.
	if (startBit) {
		// Clear the previous queue, if it contains anything
		assemblingBuffer.dispose();

		// Extract H264 video data
		size_t sizeSlice = payloadSize + 2;
		uint8_t* slice = (uint8_t*) malloc(sizeSlice);

		// Reconstruct NAL header
		slice[0] = 0x00;
		slice[1] = 0x00;
		slice[2] = 0x01;
		slice[3] = (payload[0] & 0xe0) | (payload[1] & 0x1f);
		assemblingBuffer.push(slice, sizeSlice);
	} else if (endBit) {
		// We should have a complete frame by now.
		uint8_t* videoFrame = assemblingBuffer.getContinuousBuffer();
		size_t videoFrameSize = assemblingBuffer.getSize();
	}
}

void H264Depayloader::handleSingleNalUnitMode(const ost::AppDataUnit* adu) {
	/**
	 *  0                   1                   2                   3
	 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |F|NRI|  type   |                                               |
	 * +-+-+-+-+-+-+-+-+                                               |
	 * |                                                               |
	 * |               Bytes 2..n of a Single NAL unit                 |
	 * |                                                               |
	 * |                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |                               :...OPTIONAL RTP padding        |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	const uint8_t* payload = adu->getData();
	size_t payloadSize = adu->getSize();

	size_t frameSize = payloadSize + 3;
	uint8_t* frame = (uint8_t*) malloc(frameSize);

	frame[0] = 0x00;
	frame[1] = 0x00;
	frame[2] = 0x01;
	memcpy (&frame[3], payload, payloadSize);
}

/**
 * @Override
 */
void H264Depayloader::process(const ost::AppDataUnit* adu) throw(UnsupportedPacketizationMode){
	/**
	 *
	 * The expected payload format is the following.
	 *  0                   1                   2                   3
	 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * | FU indicator  |   FU header   |                               |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               |
	 * |                                                               |
	 * |                         FU payload                            |
	 * |                                                               |
	 * |                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |                               :...OPTIONAL RTP padding        |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 *
	 * The FU indicator octet.
	 * +---------------+
	 * |0|1|2|3|4|5|6|7|
	 * +-+-+-+-+-+-+-+-+
	 * |F|NRI|  Type   |
	 * +---------------+
	 *
	 *     Type   Packet    Type name                        Section
	 * ---------------------------------------------------------
	 * 0      undefined                                    -
	 * 1-23   NAL unit  Single NAL unit packet per H.264   5.6
	 * 24     STAP-A    Single-time aggregation packet     5.7.1
	 * 25     STAP-B    Single-time aggregation packet     5.7.1
	 * 26     MTAP16    Multi-time aggregation packet      5.7.2
	 * 27     MTAP24    Multi-time aggregation packet      5.7.2
	 * 28     FU-A      Fragmentation unit                 5.8
	 * 29     FU-B      Fragmentation unit                 5.8
	 * 30-31  undefined
	 */

	const uint8* payload = adu->getData();
	uint8_t nalRefIdc = (payload[0] & 0x60) >> 5;
	uint8_t nalUnitType = payload[0] & 0x1f;

	/**
	 * A value of 00 indicates that the content of the NAL
	 * unit is not used to reconstruct reference pictures for inter picture prediction.
	 *
	 * Such NAL units can be discarded without risking the integrity of the reference pictures.
	 */
	if (nalRefIdc) {
		switch (nalUnitType) {
		case 0:
		case 24:
		case 25:
		case 26:
		case 27:
		case 30:
		case 31:
		{
			std::ostringstream msg;
			msg << "NAL Unit type ";
			msg << nalUnitType;
			msg << " is unsupported.";

			throw UnsupportedPacketizationMode(msg.str());
			break;
		}
		case 28:
			/**
			 * NAL unit type FU-B MUST be used in the interleaved packetization mode
			 * for the first fragmentation unit of a fragmented NAL unit.
			 * In other words, in the interleaved packetization mode, each NALU that
			 * is fragmented has an FU-B as the first fragment, followed by one or more FU-A fragments
			 *
			 * The structure is the same as for FU-A, except for the additional DON (Decoding Order Number) field.
			 */
		case 29:
			handleFragmentationUnit(adu);
			break;
		default:
			handleSingleNalUnitMode(adu);
			break;
		}
	}
}
}
