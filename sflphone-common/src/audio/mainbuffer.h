/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author : Alexandre Savard <alexandre.savard@savoirfairelinux.com>
 *
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

#ifndef __MAIN_BUFFER__
#define __MAIN_BUFFER__

#include <map>
#include <set>
#include <cc++/thread.h> // for ost::Mutex
#include <string>

#include "global.h"
#include "call.h"
#include "ringbuffer.h"

typedef std::map<CallId, RingBuffer*> RingBufferMap;

typedef std::set<CallId> CallIdSet;

typedef std::map<CallId, CallIdSet*> CallIdMap;

class MainBuffer
{

    public:

        MainBuffer();

        ~MainBuffer();

        void setInternalSamplingRate (int sr);

        int getInternalSamplingRate() {
            return _internalSamplingRate;
        }

        /**
         * Bind together two audio streams so taht a client will be able
         * to put and get data specifying its callid only.
         */
        void bindCallId (CallId call_id1, CallId call_id2 = default_id);

        /**
         * Add a new call_id to unidirectional outgoing stream
         * \param call_id New call id to be added for this stream
         * \param process_id Process that require this stream
         */
        void bindHalfDuplexOut (CallId process_id, CallId call_id = default_id);

        /**
         * Unbind two calls
         */
        void unBindCallId (CallId call_id1, CallId call_id2 = default_id);

        /**
         * Unbind a unidirectional stream
         */
        void unBindHalfDuplexOut (CallId process_id, CallId call_id = default_id);

        void unBindAll (CallId call_id);

        void unBindAllHalfDuplexOut (CallId process_id);

        int putData (void *buffer, int toCopy, unsigned short volume = 100, CallId call_id = default_id);

        int getData (void *buffer, int toCopy, unsigned short volume = 100, CallId call_id = default_id);

        int availForPut (CallId call_id = default_id);

        int availForGet (CallId call_id = default_id);

        int discard (int toDiscard, CallId call_id = default_id);

        void flush (CallId call_id = default_id);

        void flushAllBuffers();

        void flushDefault();

        void syncBuffers (CallId call_id);

        void stateInfo();

    private:

        CallIdSet* getCallIdSet (CallId call_id);

        bool createCallIdSet (CallId set_id);

        bool removeCallIdSet (CallId set_id);

        /**
         * Add a new call id to this set
         */
        void addCallIdtoSet (CallId set_id, CallId call_id);

        void removeCallIdfromSet (CallId set_id, CallId call_id);

        /**
         * Create a new ringbuffer with default readpointer
         */
        RingBuffer* createRingBuffer (CallId call_id);

        bool removeRingBuffer (CallId call_id);

        RingBuffer* getRingBuffer (CallId call_id);

        int getDataByID (void *buffer, int toCopy, unsigned short volume, CallId call_id, CallId reader_id);

        int availForGetByID (CallId call_id, CallId reader_id);

        int discardByID (int toDiscard, CallId call_id, CallId reader_id);

        void flushByID (CallId call_id, CallId reader_id);

        RingBufferMap _ringBufferMap;

        CallIdMap _callIDMap;

        SFLDataFormat* mixBuffer;

        ost::Mutex _mutex;

        int _internalSamplingRate;

    public:

        friend class MainBufferTest;
};

#endif
