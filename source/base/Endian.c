//
// Endian.c - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include "Endian.h"

#include "base/PlatformInfo.h"

unsigned short flipShortEndian(const unsigned short value) {
  return (value << 8) | (value >> 8);
}

unsigned int flipIntEndian(const unsigned int value) {
  return (value << 24) | ((value << 8) & 0x00ff0000) |
         ((value >> 8) & 0x0000ff00) | (value >> 24);
}

unsigned short convertBigEndianShortToPlatform(const unsigned short value) {
  return platformInfoIsLittleEndian() ? flipShortEndian(value) : value;
}

unsigned int convertBigEndianIntToPlatform(const unsigned int value) {
  return platformInfoIsLittleEndian() ? flipIntEndian(value) : value;
}

unsigned int convertLittleEndianIntToPlatform(const unsigned int value) {
  return !platformInfoIsLittleEndian() ? flipIntEndian(value) : value;
}

unsigned short convertByteArrayToUnsignedShort(const byte *value) {
  if (platformInfoIsLittleEndian()) {
    return ((value[1] << 8) & 0x0000ff00) | value[0];
  } else {
    return ((value[0] << 8) & 0x0000ff00) | value[1];
  }
}

unsigned int convertByteArrayToUnsignedInt(const byte *value) {
  if (platformInfoIsLittleEndian()) {
    return ((value[3] << 24) | ((value[2] << 16) & 0x00ff0000) |
            ((value[1] << 8) & 0x0000ff00) | value[0]);
  } else {
    return ((value[0] << 24) | ((value[1] << 16) & 0x00ff0000) |
            ((value[2] << 8) & 0x0000ff00) | value[0]);
  }
}

float convertBigEndianFloatToPlatform(const float value) {
  float result = 0.0f;
  byte *floatToConvert = (byte *)&value;
  byte *floatResult = (byte *)&result;
  floatResult[0] = floatToConvert[3];
  floatResult[1] = floatToConvert[2];
  floatResult[2] = floatToConvert[1];
  floatResult[3] = floatToConvert[0];
  return result;
}
