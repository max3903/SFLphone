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
#ifndef __SFL_PIPELINE_ELEMENT_H__
#define __SFL_PIPELINE_ELEMENT_H__

#include <list>
#include <iostream>

namespace sfl {
/**git
 * Base class for every Pipeline Element objects.
 */
template<class InputType>
class PipelineElementInterface {
public:
	/**
	 * @param data The data that is pushed from the previous element.
	 */
	void setData(InputType* data) {
		dataFromPrevious = data;
	}

protected:
	InputType* getDataFromPrevious() { return dataFromPrevious; }

private:
	InputType* dataFromPrevious;
};

/**
 * Just a fancy name for an object that intends to mimic a
 * graph-based approach for data processing.
 */
template<class InputType, class OutputType>
class PipelineElement : public PipelineElementInterface<InputType> {
public:
	/**
	 * Primary operator for executing a chain of elements.
	 */
	inline PipelineElement& operator>>(PipelineElementInterface<OutputType>& other) {
		other.setData(getData());
		return *this;
	}

	inline PipelineElement& operator>>(OutputType* data) {
		*data = *getData();
		return *this;
	}

	/**
	 * Data specific input mode
	 */
	inline PipelineElement& operator<<(InputType* data) {
		PipelineElementInterface<InputType>& parent = dynamic_cast<PipelineElementInterface<InputType>& >(*this);
		parent.setData(data);
		return *this;
	}

	/**
	 * Used internally only in the pipeline.
	 * @return The computed data for this element.
	 */
	virtual OutputType* getData() = 0;
};

}

#endif
