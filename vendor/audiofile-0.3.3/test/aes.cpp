/*
	Copyright (C) 2011, Michael Pruett. All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

	3. The name of the author may not be used to endorse or promote products
	derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <gtest/gtest.h>
#include <audiofile.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static const char *kTestFileName = "/tmp/test.aiff";

TEST(AES, AIFF)
{
	AFfilesetup setup = afNewFileSetup();
	afInitFileFormat(setup, AF_FILE_AIFF);
	afInitAESChannelDataTo(setup, AF_DEFAULT_TRACK, true);

	AFfilehandle file = afOpenFile(kTestFileName, "w", setup);
	ASSERT_TRUE(file);

	afFreeFileSetup(setup);

	unsigned char aesData[24];
	for (int i=0; i<24; i++)
		aesData[i] = 3*i + 1;
	afSetAESChannelData(file, AF_DEFAULT_TRACK, aesData);

	afCloseFile(file);

	file = afOpenFile(kTestFileName, "r", NULL);
	ASSERT_TRUE(file);

	unsigned char readAESData[24];
	EXPECT_TRUE(afGetAESChannelData(file, AF_DEFAULT_TRACK, readAESData));
	EXPECT_TRUE(!memcmp(aesData, readAESData, 24));

	afCloseFile(file);

	::unlink(kTestFileName);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
