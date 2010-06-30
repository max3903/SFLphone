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
#ifndef __SFL_ABSTRACT_OBSERVERVABLE_H__
#define __SFL_ABSTRACT_OBSERVERVABLE_H__

#include <vector>
#include <algorithm>

#include "Observable.h"
#include "Observer.h"

namespace sfl {
template<class PushedDataType, class ObserverType>
class AbstractObservable: public Observable<PushedDataType, ObserverType> {
public:
	/**
	 * @Override
	 */
	void addObserver(ObserverType* observer) {
		observers.push_back(observer);
	}

	/**
	 * @Override
	 */
	void removeObserver(ObserverType* observer) {
		std::remove(observers.begin(), observers.end(), observer);
	}

	/**
	 * @Override
	 */
	void clearObservers() {
		observers.erase(observers.begin(), observers.end());
	}

	/**
	 * @Override
	 */
	bool isObserver(ObserverType* observer) {
		if (std::find(observers.begin(), observers.end(), observer) != observers.end()) {
			return true;
		}

		return false;
	}


	/**
	 * @Override
	 */
	void notifyAll(PushedDataType data) {
		typename std::vector<ObserverType*>::iterator it;
		for (it = observers.begin(); it < observers.end(); it++) {
			notify((*it), data);
		}
	}

	/**
	 * @Override
	 */
	void notifyAll(PushedDataType data, const std::string& name) {
		typename std::vector<ObserverType*>::iterator it;
		for (it = observers.begin(); it < observers.end(); it++) {
			notify((*it), name, data);
		}
	}

private:
	std::vector<ObserverType*> observers;
};
}

#endif
