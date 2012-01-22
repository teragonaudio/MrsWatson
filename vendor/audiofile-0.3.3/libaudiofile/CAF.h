/*
	Audio File Library
	Copyright (C) 2011, Michael Pruett <michael@68k.org>

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

#ifndef CAF_H
#define CAF_H

#include "File.h"
#include "FileHandle.h"
#include "Tag.h"
#include <stdint.h>

#define _AF_CAF_NUM_COMPTYPES 2
extern const int _af_caf_compression_types[_AF_CAF_NUM_COMPTYPES];

class CAFFile : public _AFfilehandle
{
public:
	static bool recognize(File *);
	static AFfilesetup completeSetup(AFfilesetup);

	CAFFile();
	~CAFFile();

	status readInit(AFfilesetup);
	status writeInit(AFfilesetup);
	status update();

private:
	AFfileoffset m_dataOffset;
	uint32_t m_bytesPerPacket, m_framesPerPacket;

	status parseDescription(const Tag &, int64_t);
	status parseData(const Tag &, int64_t);

	status writeDescription();
	status writeData(bool update);
};

#endif
