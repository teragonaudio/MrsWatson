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

#include "config.h"
#include "Module.h"

#include "File.h"
#include "SimpleModule.h"
#include "byteorder.h"
#include "compression.h"
#include "units.h"
#include "../pcm.h"

Module::Module() :
	m_sink(NULL)
{
}

Module::~Module()
{
}

void Module::setSink(Module *module) { m_sink = module; }
void Module::setSource(Module *module) { m_source = module; }

const char *Module::name() const { return ""; }

void Module::describe()
{
}

void Module::maxPull()
{
	m_inChunk->frameCount = m_outChunk->frameCount;
}

void Module::maxPush()
{
	m_outChunk->frameCount = m_inChunk->frameCount;
}

void Module::runPull()
{
}

void Module::runPush()
{
}

void Module::pull(size_t frames)
{
	m_inChunk->frameCount = frames;
	m_source->runPull();
}

void Module::push(size_t frames)
{
	m_outChunk->frameCount = frames;
	m_sink->runPush();
}
