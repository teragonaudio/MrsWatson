/*
	Audio File Library
	Copyright (C) 1998-2000, Michael Pruett <michael@68k.org>

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

#ifndef SETUP_H
#define SETUP_H

#include "afinternal.h"

struct InstrumentSetup;
struct MiscellaneousSetup;
struct TrackSetup;

struct _AFfilesetup
{
	int	valid;

	int fileFormat;

	bool trackSet, instrumentSet, miscellaneousSet;

	int trackCount;
	TrackSetup *tracks;

	int instrumentCount;
	InstrumentSetup *instruments;

	int miscellaneousCount;
	MiscellaneousSetup *miscellaneous;

	TrackSetup *getTrack(int trackID = AF_DEFAULT_TRACK);
	InstrumentSetup *getInstrument(int instrumentID);
	MiscellaneousSetup *getMiscellaneous(int miscellaneousID);
};

AFfilesetup _af_filesetup_copy (AFfilesetup setup, AFfilesetup defaultSetup,
        bool copyMarks);

void _af_setup_free_markers (AFfilesetup setup, int trackno);
void _af_setup_free_tracks (AFfilesetup setup);
void _af_setup_free_instruments (AFfilesetup setup);
AFfilesetup _af_filesetup_copy (AFfilesetup setup, AFfilesetup defaultSetup,
        bool copyMarks);
status _af_filesetup_make_handle (AFfilesetup setup, AFfilehandle handle);

InstrumentSetup *_af_instsetup_new (int count);

#endif
