/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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

void VideoCaptureTest::setUp() 
{
	std::cout << "Setting up..." << std::endl;

	videoInput = new sfl::VideoInputSourceGst();
}

void VideoCaptureTest::tearDown() 
{
	std::cout << "Tearing down..." << std::endl;
	delete videoInput;
}

void VideoCaptureTest::testOpenClose() 
{
	std::cout << "Testing open/close" << std::endl;

	// Pick the first device (should be videotestsrc or ximage, and tries to open it
	std::vector<sfl::VideoDevice*> devices = videoInput->enumerateDevices();
	videoInput->setDevice(devices.at(0));

	 CPPUNIT_ASSERT_NO_THROW(videoInput->open(320, 240, 30));
	 CPPUNIT_ASSERT_NO_THROW(videoInput->close());
}

void VideoCaptureTest::testEnumerateDevices() 
{
	std::cout << "Testing device enumeration" << std::endl;

	// Expecting that the container is at least not empty
	// ximagesrc and videotestsrc should minimally be available.

	std::vector<sfl::VideoDevice*> devices = videoInput->enumerateDevices();
	std::vector<sfl::VideoDevice*>::iterator it;
	for (it = devices.begin(); it < devices.end(); it++) {
		std::cout << "Name: " + (*it)->getName() << std::endl;
		std::cout << "Description: " + (*it)->getDescription() << std::endl;
	}

	CPPUNIT_ASSERT(devices.size() > 0);
}

void VideoCaptureTest::testFrameObserver()
{
	std::cout << "Testing frame observer pattern" << std::endl;

	// Define a new observer
	VideoFrameObserverTest observer;

	std::vector<sfl::VideoDevice*> devices = videoInput->enumerateDevices();

	videoInput->setDevice(devices.at(0));

	videoInput->addVideoFrameObserver(&observer);

	videoInput->open(320, 240, 30);

	// Let some frames be captured.
	sleep(2);

	videoInput->close();

	CPPUNIT_ASSERT(observer.i > 0);
}

void VideoCaptureTest::testGrabFrame()
{
	std::cout << "Testing grabFrame()" << std::endl;

	std::vector<sfl::VideoDevice*> devices = videoInput->enumerateDevices();
	videoInput->setDevice(devices.at(0));

	videoInput->open(320, 240, 30);

	CPPUNIT_ASSERT_NO_THROW(videoInput->grabFrame());
	sfl::VideoFrame * frame = videoInput->getCurrentFrame();
	CPPUNIT_ASSERT(frame != NULL);

	videoInput->close();
}

