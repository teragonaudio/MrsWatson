//
// BuildInfo.h - MrsWatson
// Created by Nik Reiman on 1/2/12.
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

#ifndef MrsWatson_BuildInfo_h
#define MrsWatson_BuildInfo_h

#include "base/CharString.h"

#define PROGRAM_NAME "MrsWatson"
#define VENDOR_NAME "Teragon Audio"

#define VERSION_MAJOR 0
#define VERSION_MINOR 9
#define VERSION_PATCH 7

#define OFFICIAL_WEBSITE "http://www.teragonaudio.com"
#define PROJECT_WEBSITE "https://github.com/teragonaudio/mrswatson"
#define SUPPORT_WEBSITE "https://github.com/teragonaudio/mrswatson/issues"
#define SUPPORT_EMAIL "support@teragonaudio.com"

#define LICENSE_STRING "Redistribution and use in source and binary forms, with or without " \
"modification, are permitted provided that the following conditions are met:\n\n" \
"* Redistributions of source code must retain the above copyright notice, this list of " \
"conditions and the following disclaimer.\n" \
"* Redistributions in binary form must reproduce the above copyright notice, this list of " \
"conditions and the following disclaimer in the documentation and/or other materials " \
"provided with the distribution.\n\n" \
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY " \
"EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF " \
"MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE " \
"COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, " \
"EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE " \
"GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED " \
"AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING " \
"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED " \
"OF THE POSSIBILITY OF SUCH DAMAGE."

/**
 * @return Year the application was built in
 */
unsigned long buildInfoGetYear(void);

/**
 * @return Build timestamp in the form YYYYMMDD
 */
unsigned long buildInfoGetDatestamp(void);

/**
 * Get the full application name and version
 * @return String with the application's name and version. The caller must free
 * this memory when finished with it.
 */
CharString buildInfoGetVersionString(void);

#endif
