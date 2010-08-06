/*
 *  Copyright (C) 2006-2009 Savoir-Faire Linux inc.
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
#ifndef __SFL_SEMAPHORE_POSIX__
#define __SFL_SEMAPHORE_POSIX__

#include <time.h>
#include <string>
#include <stdexcept>
#include <semaphore.h>

namespace sfl
{

class SemaphorePosixException: public std::runtime_error
{
    public:
        SemaphorePosixException (std::string const& msg) :
                std::runtime_error (msg) {
        }
};

class SemaphorePosix
{
    public:
        /**
         * Creates a new named posix semaphore object.
         * @param name The name to be used as a reference to the semaphore object.
         * @param exclusive If set to true, if the semaphore exists, the constructor will fail.
         * @param perms Control the permissions. Eg : 0644
         * @param value Initial value of the semaphore.
         */
        SemaphorePosix (const std::string& name, bool exclusive, mode_t perms,
                        unsigned value) throw (SemaphorePosixException) ;

        /**
         * Creates a new unamed posix semaphore object.
         * @param shared Indicates whether this semaphore is to be shared between the threads of a process, or between processes.
         * @param value Specifies the initial value for the semaphore.
         */
        SemaphorePosix (bool shared, unsigned value) throw (SemaphorePosixException) ;

        /**
         * Closes the semaphore. If the semaphore needs to be removed
         * from the system, this can be handled automatically and unlink()
         * should be called.
         */
        ~SemaphorePosix();

        /**
         * Wait on the semaphore to acquire a unit value.
         */
        void wait() throw (SemaphorePosixException) ;

        /**
         * Same as wait(). However, it does not have
         * a blocking behaviour and returns instead.
         * @return False if not available, true otherwise.
         * @sa wait()
         */
        bool tryWait() throw (SemaphorePosixException) ;

        /**
         * Blocking wait behavior, but returns after some specified amount
         * of time.
         * @param sec The seconds value.
         * @param nsec The nano seconds component.
         * @return false if wait() blocked for some greater amount of time than the specified quantity.
         */
        bool timedWait (time_t sec, long nsec = 0) throw (SemaphorePosixException) ;

        /**
         * Release/signal a unit value of the semaphore.
         */
        void post() throw (SemaphorePosixException) ;

        /**
         * Get the current value in the semaphore.
         */
        int getValue() throw (SemaphorePosixException) ;

        /**
         * Removes/destroys the semaphore from the system.
         */
        void remove() throw (SemaphorePosixException) ;

    private:
        sem_t * sem;
        std::string name;
};
}

#endif
