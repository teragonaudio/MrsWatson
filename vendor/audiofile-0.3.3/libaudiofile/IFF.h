/*
	Audio File Library
	Copyright (C) 2004, Michael Pruett <michael@68k.org>

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
	IFF.h

	This file declares constants and functions related to the
	IFF/8SVX file format.
*/

#ifndef IFF_H
#define IFF_H

#include "FileHandle.h"

class IFFFile : public _AFfilehandle
{
public:
	static bool recognize(File *fh);
	static AFfilesetup completeSetup(AFfilesetup);

	IFFFile();
	status readInit(AFfilesetup);
	status writeInit(AFfilesetup);
	status update();

private:
	AFfileoffset	miscellaneousPosition;
	AFfileoffset	VHDR_offset;
	AFfileoffset	BODY_offset;

	status parseMiscellaneous(const Tag &type, size_t size);
	status parseVHDR(const Tag &type, size_t size);
	status parseBODY(const Tag &type, size_t size);

	status writeVHDR();
	status writeMiscellaneous();
	status writeBODY();
};

#endif
