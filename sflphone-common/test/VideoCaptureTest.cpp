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
#include "VideoCaptureTest.h"

#include "logger.h"
#include "video/VideoInputSourceGst.h"
#include "video/VideoInputSource.h"
#include "video/VideoDevice.h"
#include "video/FrameFormat.h"

void VideoCaptureTest::setUp() {
	std::cout << "Setting up..." << std::endl;

	videoInput = new sfl::VideoInputSourceGst();
}

void VideoCaptureTest::tearDown() {
	std::cout << "Tearing down..." << std::endl;
	delete videoInput;
}

void VideoCaptureTest::testOpenClose() {
	std::cout << "Testing open/close" << std::endl;

	// VideoDeviceIOException, NoVideoDeviceAvailableException
	CPPUNIT_ASSERT_NO_THROW(videoInput->open());
	sleep(2);
	CPPUNIT_ASSERT_NO_THROW(videoInput->close());
}

void VideoCaptureTest::testEnumerateDevices() {
	std::cout << "Testing device enumeration" << std::endl;

	// Expecting that the container is at least not empty
	// ximagesrc and videotestsrc should minimally be available.

	std::vector<sfl::VideoDevicePtr> devices = videoInput->enumerateDevices();
	std::vector<sfl::VideoDevicePtr>::iterator itDevice;
	for (itDevice = devices.begin(); itDevice < devices.end(); itDevice++) {
		std::cout << "Name: " + (*itDevice)->getName() << std::endl;
		std::cout << "Device: " + (*itDevice)->getDevice() << std::endl;

		std::cout << "Preferred Frame Format: " << (*itDevice)->getPreferredFormat().toString() << std::endl;
		std::cout << "Supported Frame Formats:" << std::endl;

		std::vector<sfl::FrameFormat> formats = (*itDevice)->getSupportedFormats();
		std::vector<sfl::FrameFormat>::iterator itFormat;
		for (itFormat = formats.begin(); itFormat < formats.end(); itFormat++) {
			std::cout << "	- " << (*itFormat).toString() << std::endl;
			std::cout << "	All Supported Frame Rates: " << std::endl;

			std::vector<sfl::FrameRate> rates = (*itFormat).getFrameRates();
			std::vector<sfl::FrameRate>::iterator itRate;
			for (itRate = rates.begin(); itRate < rates.end(); itRate++) {
				std::cout << "		" << (*itRate).toString() << std::endl;
			}
		}
	}

	CPPUNIT_ASSERT(devices.size() > 0);
}

void VideoCaptureTest::testFrameObserver() {
	std::cout << "Testing frame observer pattern" << std::endl;

	// Define a new observer
	VideoFrameObserverTest observer;

	std::vector<sfl::VideoDevicePtr> devices = videoInput->enumerateDevices();

	videoInput->addVideoFrameObserver(&observer);
	videoInput->open();

	// Let some frames be captured.
	sleep(2);

	videoInput->close();

	CPPUNIT_ASSERT(observer.i > 0);
}
