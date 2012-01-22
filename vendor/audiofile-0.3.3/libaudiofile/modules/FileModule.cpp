/*
	Audio File Library
	Copyright (C) 2010, Michael Pruett <michael@68k.org>

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the
	Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA  02111-1307  USA.
*/

#include "config.h"
#include "FileModule.h"

#include "File.h"
#include "Track.h"

FileModule::FileModule(Mode mode, Track *track, File *fh, bool canSeek) :
	m_mode(mode),
	m_track(track),
	m_fh(fh),
	m_canSeek(canSeek)
{
	track->fpos_next_frame = track->fpos_first_frame;
	track->frames2ignore = 0;
}

ssize_t FileModule::read(void *data, size_t nbytes)
{
	ssize_t bytesRead = m_fh->read(data, nbytes);
	if (bytesRead > 0)
	{
		m_track->fpos_next_frame += bytesRead;
	}
	else if (bytesRead < 0)
	{
		m_track->filemodhappy = false;
	}
	return bytesRead;
}

ssize_t FileModule::write(const void *data, size_t nbytes)
{
	ssize_t bytesWritten = m_fh->write(data, nbytes);
	if (bytesWritten > 0)
	{
		m_track->fpos_next_frame += bytesWritten;
		m_track->data_size += bytesWritten;
	}
	else if (bytesWritten < 0)
	{
		m_track->filemodhappy = false;
	}
	return bytesWritten;
}

off_t FileModule::tell()
{
	return m_fh->tell();
}
