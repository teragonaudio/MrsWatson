/*
	Audio File Library
	Copyright (C) 2000, Silicon Graphics, Inc.

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

/*
	Raw.h
*/

#ifndef RAW_H
#define RAW_H

#include "FileHandle.h"

#define _AF_RAW_NUM_COMPTYPES 2
extern const int _af_raw_compression_types[_AF_RAW_NUM_COMPTYPES];

class RawFile : public _AFfilehandle
{
public:
	static bool recognize(File *fh);
	static AFfilesetup completeSetup(AFfilesetup);

	status readInit(AFfilesetup setup);
	status writeInit(AFfilesetup setup);
	status update();
};

#endif
