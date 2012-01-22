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
	NIST.h

	This file declares code for reading and writing NIST SPHERE files.
*/

#ifndef NIST_H
#define NIST_H

#include "FileHandle.h"

class NISTFile : public _AFfilehandle
{
public:
	static bool recognize(File *fh);
	static AFfilesetup completeSetup(AFfilesetup setup);

	virtual status readInit(AFfilesetup);
	virtual status writeInit(AFfilesetup);
	virtual status update();

private:
	status writeHeader();
};

#endif
