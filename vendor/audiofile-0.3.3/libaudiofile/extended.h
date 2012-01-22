/*
	Audio File Library
	Copyright (C) 1998, Michael Pruett <michael@68k.org>

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
	extended.h

	This file defines interfaces to Apple's extended floating-point
	conversion routines.
*/

#ifndef EXTENDED_H
#define EXTENDED_H

#ifdef __cplusplus
extern "C" {
#endif

void _af_convert_to_ieee_extended (double num, unsigned char *bytes);
double _af_convert_from_ieee_extended (const unsigned char *bytes);

#ifdef __cplusplus
}
#endif

#endif
