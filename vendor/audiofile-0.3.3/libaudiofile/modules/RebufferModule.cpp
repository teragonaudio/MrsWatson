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
#include "RebufferModule.h"

#include <algorithm>
#include <assert.h>
#include <string.h>

RebufferModule::RebufferModule(Direction direction, int channelCount, int numFrames, bool multipleOf) :
	m_direction(direction),
	m_channelCount(channelCount),
	m_numFrames(numFrames),
	m_multipleOf(multipleOf),
	m_eof(false),
	m_sentShortChunk(false)
{
	if (m_direction == FixedToVariable)
		initFixedToVariable();
	else
		initVariableToFixed();
}

void RebufferModule::initFixedToVariable()
{
	m_offset = m_numFrames;
	m_buffer.reset(new element_type[m_numFrames * m_channelCount]);
}

void RebufferModule::initVariableToFixed()
{
	m_offset = 0;
	m_buffer.reset(new element_type[m_numFrames * m_channelCount]);
	m_savedBuffer.reset(new element_type[m_numFrames * m_channelCount]);
}

void RebufferModule::maxPull()
{
	assert(m_direction == FixedToVariable);
	if (m_multipleOf)
		m_inChunk->frameCount = m_outChunk->frameCount + m_numFrames;
	else
		m_inChunk->frameCount = m_numFrames;
}

void RebufferModule::maxPush()
{
	assert(m_direction == VariableToFixed);
	if (m_multipleOf)
		m_outChunk->frameCount = m_inChunk->frameCount + m_numFrames;
	else
		m_outChunk->frameCount = m_numFrames;
}

void RebufferModule::runPull()
{
	int framesToPull = m_outChunk->frameCount;
	const element_type *inBuffer = static_cast<const element_type *>(m_inChunk->buffer);
	element_type *outBuffer = static_cast<element_type *>(m_outChunk->buffer);

	assert(m_offset > 0 && m_offset <= m_numFrames);

	/*
		A module should not pull more frames from its input
		after receiving a short chunk.
	*/
	assert(!m_sentShortChunk);

	if (m_offset < m_numFrames)
	{
		int buffered = m_numFrames - m_offset;
		int n = std::min(framesToPull, buffered);
		memcpy(outBuffer, m_buffer.get() + m_offset * m_channelCount,
			sizeof (element_type) * n * m_channelCount);
		outBuffer += buffered * m_channelCount;
		framesToPull -= buffered;
		m_offset += n;
	}

	// Try to pull more frames from the source.
	while (!m_eof && framesToPull > 0)
	{
		int framesRequested;
		if (m_multipleOf)
			/* Round framesToPush up to nearest multiple of m_numFrames */
			framesRequested = ((framesToPull - 1) / m_numFrames + 1) * m_numFrames;
		else
			framesRequested = m_numFrames;

		assert(framesRequested > 0);

		pull(framesRequested);

		int framesReceived = m_inChunk->frameCount;

		if (framesReceived != framesRequested)
			m_eof = true;

		memcpy(outBuffer, inBuffer,
			sizeof (element_type) * std::min(framesToPull, framesReceived) * m_channelCount);

		outBuffer += framesReceived * m_channelCount;
		framesToPull -= framesReceived;

		if (m_multipleOf)
			assert(m_eof || framesToPull <= 0);

		if (framesToPull < 0)
		{
			assert(m_offset == m_numFrames);

			m_offset = m_numFrames + framesToPull;

			assert(m_offset > 0 && m_offset <= m_numFrames);

			memcpy(m_buffer.get() + m_offset * m_channelCount,
				inBuffer + m_offset * m_channelCount,
				sizeof (element_type) * (m_numFrames - m_offset) * m_channelCount);
		}
		else
		{
			assert(m_offset == m_numFrames);
		}
	}

	if (m_eof && framesToPull > 0)
	{
		// Output short chunk.
		m_outChunk->frameCount -= framesToPull;
		m_sentShortChunk = true;
		assert(m_offset == m_numFrames);
	}
	else
	{
		assert(framesToPull <= 0);
		assert(m_offset == m_numFrames + framesToPull);
	}
	assert(m_offset > 0 && m_offset <= m_numFrames);
}

void RebufferModule::reset1()
{
	m_offset = m_numFrames;
	m_eof = false;
	m_sentShortChunk = false;
	assert(m_offset > 0 && m_offset <= m_numFrames);
}

void RebufferModule::reset2()
{
#ifdef DEBUG
	assert(m_offset > 0 && m_offset <= m_numFrames);
#endif
}

void RebufferModule::runPush()
{
	int framesToPush = m_inChunk->frameCount;
	const element_type *inBuffer = static_cast<const element_type *>(m_inChunk->buffer);
	element_type *outBuffer = static_cast<element_type *>(m_outChunk->buffer);

	assert(m_offset >= 0 && m_offset < m_numFrames);

	// Check that we will be able to push even one block.
	if (m_offset + framesToPush >= m_numFrames)
	{
		if (m_offset > 0)
			memcpy(m_outChunk->buffer, m_buffer.get(),
				sizeof (element_type) * m_offset * m_channelCount);

		if (m_multipleOf)
		{
			// Round down to nearest multiple of m_numFrames.
			int n = ((m_offset + framesToPush) / m_numFrames) * m_numFrames;

			assert(n > m_offset);
			memcpy(outBuffer + m_offset * m_channelCount,
				inBuffer,
				sizeof (element_type) * (n - m_offset) * m_channelCount);

			push(n);

			inBuffer += (n - m_offset) * m_channelCount;
			framesToPush -= n - m_offset;
			assert(framesToPush >= 0);
			m_offset = 0;
		}
		else
		{
			while (m_offset + framesToPush >= m_numFrames)
			{
				int n = m_numFrames - m_offset;
				memcpy(outBuffer + m_offset * m_channelCount,
					inBuffer,
					sizeof (element_type) * n * m_channelCount);

				push(m_numFrames);

				inBuffer += n * m_channelCount;
				framesToPush -= n;
				assert(framesToPush >= 0);
				m_offset = 0;
			}
		}

		assert(m_offset == 0);
	}

	assert(m_offset + framesToPush < m_numFrames);

	// Save remaining samples in buffer.
	if (framesToPush > 0)
	{
		memcpy(m_buffer.get() + m_offset * m_channelCount,
			inBuffer,
			sizeof (element_type) * framesToPush * m_channelCount);
		m_offset += framesToPush;
	}

	assert(m_offset >= 0 && m_offset < m_numFrames);
}

void RebufferModule::sync1()
{
	assert(m_offset >= 0 && m_offset < m_numFrames);

	// Save all the frames and the offset so we can restore our state later.
	memcpy(m_savedBuffer.get(), m_buffer.get(),
		sizeof (element_type) * m_numFrames * m_channelCount);
	m_savedOffset = m_offset;
}

void RebufferModule::sync2()
{
	assert(m_offset >= 0 && m_offset < m_numFrames);

	memcpy(m_outChunk->buffer, m_buffer.get(),
		sizeof (element_type) * m_offset * m_channelCount);

	push(m_offset);

	memcpy(m_buffer.get(), m_savedBuffer.get(),
		sizeof (element_type) * m_numFrames * m_channelCount);
	m_offset = m_savedOffset;

	assert(m_offset >= 0 && m_offset < m_numFrames);
}
