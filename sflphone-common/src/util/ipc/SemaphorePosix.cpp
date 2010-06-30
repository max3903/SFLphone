/*
 *  Copyright (C) 2006-2010 Savoir-Faire Linux inc.
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

#include "SemaphorePosix.h"

#include <string>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#include <iostream>
namespace sfl {

SemaphorePosix::SemaphorePosix(const std::string& name, bool exclusive,
		mode_t perms, unsigned value) throw(SemaphorePosixException)  :
	sem(NULL), name(name) {
	int oflags = O_CREAT;
	if (exclusive) {
		oflags |= O_EXCL;
	}

	sem = sem_open(name.c_str(), oflags, perms, value);
	if (sem == SEM_FAILED) {
		sem_unlink(name.c_str());
		throw SemaphorePosixException(std::string("sem_open(): ") + strerror(
				errno));
	}
}

SemaphorePosix::SemaphorePosix(bool shared, unsigned value) throw(SemaphorePosixException)  :
	sem(NULL), name(name) {
	if (sem_init(sem, (int) shared, value) < 0) {
		throw SemaphorePosixException(std::string("sem_init(): ") + strerror(
				errno));
	}
}

SemaphorePosix::~SemaphorePosix() {
}

void SemaphorePosix::wait() throw(SemaphorePosixException)  {
	assert(sem);

	// A signal handler always interrupts a wait call
	int ret = 0;
	while ((ret = sem_wait(sem)) == EINTR)
		continue;

	if (ret < 0) {
		throw SemaphorePosixException(std::string("sem_wait(): ") + strerror(
				errno));
	}
}

bool SemaphorePosix::timedWait(time_t sec, long nsec) throw(SemaphorePosixException)  {
	assert(sem);

	struct timespec ts;
	ts.tv_sec = sec;
	ts.tv_nsec = nsec;

	int ret = 0;
	while ((ret = sem_timedwait(sem, &ts)) == EINTR)
		continue;

	if (ret == ETIMEDOUT) {
		return false;
	} else if (ret < 0) {
		throw SemaphorePosixException(std::string("sem_timedwait(): ")
				+ strerror(errno));
	}

	return true;
}

bool SemaphorePosix::tryWait() throw(SemaphorePosixException)  {
	assert(sem);
	int ret = 0;

	do {
		ret = sem_trywait(sem);
	} while (errno == EINTR);

	if (errno == EAGAIN) {
		return false;
	} else if (ret < 0) {
		throw SemaphorePosixException(std::string("sem_trywait(): ") + strerror(
				errno));
	}

	return true;
}

void SemaphorePosix::post() throw(SemaphorePosixException)  {
	assert(sem);
	if (sem_post(sem) < 0) {
		throw SemaphorePosixException(std::string("sem_post(): ") + strerror(
				errno));
	}
}

int SemaphorePosix::getValue() throw(SemaphorePosixException)  {
	assert(sem);
	int value = 0;
	if (sem_getvalue(sem, &value) < 0) {
		throw SemaphorePosixException(std::string("sem_getvalue(): ")
				+ strerror(errno));
	}

	return value;
}

void SemaphorePosix::remove() throw(SemaphorePosixException) {
	if (name.length()) {
		if (sem_unlink(name.c_str()) < 0) {
			throw SemaphorePosixException(std::string("sem_unlink(): ")
					+ strerror(errno));
		}
	} else {
		if (sem_destroy(sem) < 0) {
			throw SemaphorePosixException(std::string("sem_destroy(): ")
					+ strerror(errno));
		}
	}

	sem = NULL;
	name = "";
}

}
