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

#ifndef FILE_MODULE_H
#define FILE_MODULE_H

#include "Module.h"

class FileModule : public Module
{
protected:
	enum Mode { Compress, Decompress };
	FileModule(Mode, Track *, File *fh, bool canSeek);

	Mode mode() const { return m_mode; }
	bool canSeek() const { return m_canSeek; }

	ssize_t read(void *data, size_t nbytes);
	ssize_t write(const void *data, size_t nbytes);
	off_t tell();

private:
	Mode m_mode;

protected:
	Track *m_track;

private:
	File *m_fh;
	bool m_canSeek;
};

#endif // FILE_MODULE_H
