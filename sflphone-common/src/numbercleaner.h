/*
 *  Copyright (C) 2009 Savoir-Faire Linux inc.
 *
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

#ifndef _NUMBER_CLEANER_H
#define _NUMBER_CLEANER_H

#include <string>

class NumberCleaner {

    public:
        NumberCleaner (void);
        ~NumberCleaner (void);

        std::string clean (std::string to_clean);

        inline void set_phone_number_prefix (std::string prefix) { _prefix = prefix; }

        inline std::string get_phone_number_prefix (void) { return _prefix; }

    private:

        std::string _prefix;

        void strip_char(std::string to_strip, std::string *num);

};

#endif