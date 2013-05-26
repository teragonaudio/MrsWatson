//
// StringUtilities.c - MrsWatson
// Created by Nik Reiman on 1/4/12.
// Copyright (c) 2012 Teragon Audio. All rights reserved.
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "base/PlatformUtilities.h"
#include "base/StringUtilities.h"

boolByte isLetter(char ch) {
  return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

boolByte isNumber(char ch) {
  return (ch >= '0' && ch <= '9');
}

CharString convertIntIdToString(const unsigned long id) {
  CharString result = newCharStringWithCapacity(5);
  int i;
  for(i = 0; i < 4; i++) {
    result->data[i] = (char)(id >> ((3 - i) * 8) & 0xff);
  }
  return result;
}

unsigned long convertStringIdToInt(const CharString id) {
  unsigned long result = 0;
  int i;
  if(id != NULL && strlen(id->data) == 4) {
    for(i = 0; i < 4; i++) {
      result |= (unsigned long)(id->data[i]) << ((3 - i) * 8);
    }
  }
  return result;
}
