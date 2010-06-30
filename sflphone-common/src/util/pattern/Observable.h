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
#ifndef __SFL_OBSERVERVABLE_H__
#define __SFL_OBSERVERVABLE_H__

#include "Observer.h"

namespace sfl {
/**
 * Interface for an Observable type.
 */
template<class PushedDataType, class ObserverType>
class Observable {
public:
	/**
	 * @param observer The observer object to be notify by this observable object.
	 */
	virtual void addObserver(ObserverType* observer) = 0;

	/**
	 * @param observer The observer object to be removed.
	 */
	virtual void removeObserver(ObserverType* observer) = 0;

	/**
	 * @postcondition The observer list will be empty.
	 */
	virtual void clearObservers() = 0;

	/**
	 * @param observer The observer to check for.
	 * @return true if the given observer is currently registered on this subject.
	 */
	virtual bool isObserver(ObserverType* observer) = 0;

	/**
	 * @param data The data to be pushed to the observers.
	 */
	virtual void notifyAll(PushedDataType data) = 0;

	/**
	 * @param data The data to be pushed to the observers.
	 * @param name The method name to call.
	 */
	virtual void notifyAll(PushedDataType data, const std::string& name) = 0;

protected:
	/**
	 * This method must be overridden by the user, as in the template design pattern.
	 * The implementer will use this as a way to call the appropriate "notify()" type of method
	 * on the observer. That way, multiple inheritance on the Observer derived types won't cause
	 * any conflicts.
	 * @see AbstractObservable#notifyAll
	 */
	void notify(ObserverType* observer, PushedDataType data) {};

	/**
	 * Optional method that the implementer can override in order to call a specific method
	 * on the observer.
	 * @param name The method name to call.
	 */
	void notify(ObserverType* observer, const std::string& name, PushedDataType data) {};
};
}
#endif
