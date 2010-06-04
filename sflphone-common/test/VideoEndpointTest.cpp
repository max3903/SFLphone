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
#include "VideoEndpointTest.h"

void VideoEndpointTest::setUp()
{
	std::cout << "Setting up..." << std::endl;

	// Define the input source
	videoSource = new sfl::VideoInputSourceGst();
	videoSource->setDevice();

	videoEndpoint = new sfl::VideoEndpoint(videoSource);
}

void VideoEndpointTest::tearDown()
{
	std::cout << "Tearing down..." << std::endl;

	delete videoSource;
	delete videoEndpoint;
}

void VideoEndpointTest::shmWriteTest()
{
	std::cout << "Writing frames for few seconds ..." << std::endl;

	videoSource->open();

	// Some frames should be grabbed and written into shm ...
	sleep(2);

	videoSource->close();
}


