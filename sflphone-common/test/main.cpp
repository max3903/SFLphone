/*
 *  Copyright (C) 2004-2007 Savoir-Faire Linux inc.
 *  Author: Julien Bonjean <julien.bonjean@savoirfairelinux.com>
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

#include "logger.h"
#include "global.h"
#include "manager.h"
#include "constants.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TextTestRunner.h>

// TODO: Why some header cannot be included ?
#include "accounttest.h"
#include "audiolayertest.h"
#include "configurationtest.h"
//#include "historytest.h"
//#include "hookmanagertest.h"
#include "mainbuffertest.h"
#include "numbercleanertest.h"
//#include "pluginmanagertest.h"
//#include "rtptest.h"
#include "sdesnegotiatortest.h"


int main(int argc, char* argv[]) {

	printf("\nSFLphone Daemon Test Suite, by Savoir-Faire Linux 2004-2010\n\n");

	Logger::setConsoleLog(true);

	Logger::setDebugMode(true);
	/*
	Logger::setDebugMode(false);

	int argvIndex = 1;

	if (argc > 1) {
		if (strcmp("--help", argv[1]) == 0) {
			argvIndex++;

			CPPUNIT_NS::Test
					*suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry(
							"All Tests").makeTest();
			int testSuiteCount = suite->getChildTestCount();

			printf("Usage: test [OPTIONS] [TEST_SUITE]\n");
			printf("\nOptions:\n");
			printf(" --debug - Debug mode\n");
			printf(" --help - Print help\n");
			printf("\nAvailable test suites:\n");
			for (int i = 0; i < testSuiteCount; i++) {
				printf(" - %s\n", suite->getChildTestAt(i)->getName().c_str());
			}
			exit(0);
		}
		else if (strcmp("--debug", argv[1]) == 0) {
			argvIndex++;

			Logger::setDebugMode(true);
			_info("Debug mode activated");
		}
	}

	std::string testSuiteName = "All Tests";
	if(argvIndex < argc)
	{
		testSuiteName = argv[argvIndex];
		argvIndex++;
	}

	printf("\n\n=== SFLphone initialization ===\n\n");
	Manager::instance().initConfigFile(true, CONFIG_SAMPLE);
	Manager::instance().init();

	printf("\n\n=== Test Suite: %s ===\n\n", testSuiteName.c_str());
	// Get the top level suite from the registry
	CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry(testSuiteName).makeTest();
	*/
	CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

	/*
	if(suite->getChildTestCount() == 0)
	{
		_error("Invalid test suite name: %s", testSuiteName.c_str());
		exit(-1);
	}
	*/
	Manager::instance().initConfigFile(true, CONFIG_SAMPLE);
		Manager::instance().init();

	// Adds the test to the list of test to run
	CppUnit::TextTestRunner runner;
	runner.addTest(suite);

	// Change the default outputter to a compiler error format outputter
	runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(),
			std::cerr));
	// Run the tests.
	bool wasSucessful = runner.run();

	// Return error code 1 if the one of test failed.
	return wasSucessful ? 0 : 1;

	Manager::instance().terminate();
}