/*
	Audio File Library
	Copyright (C) 2000, Silicon Graphics, Inc.
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

#ifndef REBUFFER_MODULE_H
#define REBUFFER_MODULE_H

#include "Module.h"
#include "ScopedArray.h"

class RebufferModule : public Module
{
public:
	enum Direction
	{
		FixedToVariable,
		VariableToFixed
	};

	RebufferModule(Direction, int channelCount, int numFrames, bool multipleOf);

	virtual const char *name() const { return "rebuffer"; }

	virtual void maxPull();
	virtual void maxPush();

	virtual void runPull();
	virtual void reset1();
	virtual void reset2();
	virtual void runPush();
	virtual void sync1();
	virtual void sync2();

private:
	typedef int16_t element_type;

	Direction m_direction;
	int m_channelCount;
	int m_numFrames;
	bool m_multipleOf; // buffer to multiple of m_numFrames
	bool m_eof; // end of input stream reached
	bool m_sentShortChunk; // end of input stream indicated
	ScopedArray<element_type> m_buffer;
	int m_offset;
	ScopedArray<element_type> m_savedBuffer;
	int m_savedOffset;

	void initFixedToVariable();
	void initVariableToFixed();
};

#endif // REBUFFER_MODULE_H
