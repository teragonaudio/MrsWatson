//
// PluginPresetFxp.h - MrsWatson
// Created by Nik Reiman on 1/13/12.
// Copyright (c) 2011 Teragon Audio. All rights reserved.
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

#ifndef MrsWatson_PluginPresetFxp_h
#define MrsWatson_PluginPresetFxp_h

#include <stdio.h>

#include "plugin/PluginPreset.h"

typedef enum {
    FXP_TYPE_INVALID,
    FXP_TYPE_REGULAR,
    FXP_TYPE_OPAQUE_CHUNK,
} PluginPresetFxpProgramType;

// Copied from the VST SDK. Yes, this is a bit lame, but otherwise the C++ "virus"
// starts to leak into the code again, and as vstfxstore.h is pure C, I don't see
// any reason why all these files must be compiled as C++.
typedef struct {
    unsigned int chunkMagic;    ///< 'CcnK'
    unsigned int byteSize;      ///< size of this chunk, excl. magic + byteSize

    unsigned int fxMagic;     ///< 'FxCk' (regular) or 'FPCh' (opaque chunk)
    unsigned int version;     ///< format version (currently 1)
    unsigned int fxID;        ///< fx unique ID
    unsigned int fxVersion;     ///< fx version

    unsigned int numParams;     ///< number of parameters
    char prgName[28];     ///< program name (null-terminated ASCII string)

    union {
        float *params;    ///< variable sized array with parameter values
        struct {
            unsigned int size;    ///< size of program data
            char *chunk;    ///< variable sized array with opaque program data
        } data;         ///< program chunk data
    } content;          ///< program content depending on fxMagic
} FxpProgramMembers;
typedef FxpProgramMembers *FxpProgram;

typedef struct {
    FILE *fileHandle;
    byte *chunk;
} PluginPresetFxpDataMembers;
typedef PluginPresetFxpDataMembers *PluginPresetFxpData;

PluginPreset newPluginPresetFxp(const CharString presetName);

#endif
