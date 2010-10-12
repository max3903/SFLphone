/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoifairelinux.com>
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

#include "VideoCodecList.h"
#include "codeclibrary.h"

G_DEFINE_TYPE (VideoCodecList, video_codec_list, SFL_TYPE_CODEC_LIST)

static void
video_codec_list_init (VideoCodecList *self)
{
    /* Nothing */
}

static void
video_codec_list_class_init (VideoCodecListClass *klass)
{
    CodecListClass* base_class = SFL_CODEC_LIST_CLASS (klass);

    base_class->load_codecs = codec_library_load_video_codecs_by_account;
    base_class->save_codecs = codec_library_set_video;
    base_class->get_codecs = codec_library_get_video_codecs;
}

VideoCodecList*
video_codec_list_new (account_t* account)
{
    return g_object_new (SFL_TYPE_VIDEO_CODEC_LIST, "account", account, NULL);
}

