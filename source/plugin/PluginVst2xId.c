//
// PluginVst2xId.c - MrsWatson
// Created by Nik Reiman on 07 Jun 13.
// Copyright (c) 2013 Teragon Audio. All rights reserved.
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

#include "base/CharString.h"
#include "plugin/PluginVst2xId.h"

static CharString _convertIntIdToString(const unsigned long id)
{
    CharString result = newCharStringWithCapacity(5);
    int i;

    for (i = 0; i < 4; i++) {
        result->data[i] = (char)(id >> ((3 - i) * 8) & 0xff);
    }

    return result;
}

static unsigned long _convertStringIdToInt(const CharString idString)
{
    unsigned long result = 0;
    int i;

    if (idString != NULL && strlen(idString->data) == 4) {
        for (i = 0; i < 4; i++) {
            result |= (unsigned long)(idString->data[i]) << ((3 - i) * 8);
        }
    }

    return result;
}

PluginVst2xId newPluginVst2xId(void)
{
    PluginVst2xId pluginVst2xId = (PluginVst2xId)malloc(sizeof(PluginVst2xIdMembers));

    pluginVst2xId->id = 0;
    pluginVst2xId->idString = newCharStringWithCString(PLUGIN_VST2X_ID_UNKNOWN);

    return pluginVst2xId;
}

PluginVst2xId newPluginVst2xIdWithId(unsigned long id)
{
    PluginVst2xId pluginVst2xId = newPluginVst2xId();

    pluginVst2xId->id = id;
    freeCharString(pluginVst2xId->idString);
    pluginVst2xId->idString = _convertIntIdToString(id);

    return pluginVst2xId;
}

PluginVst2xId newPluginVst2xIdWithStringId(const CharString idString)
{
    PluginVst2xId pluginVst2xId = newPluginVst2xId();

    pluginVst2xId->id = _convertStringIdToInt(idString);

    if (idString != NULL && pluginVst2xId->id > 0) {
        charStringCopy(pluginVst2xId->idString, idString);
    }

    return pluginVst2xId;
}

void freePluginVst2xId(PluginVst2xId self)
{
    if (self) {
        freeCharString(self->idString);
        free(self);
    }
}
