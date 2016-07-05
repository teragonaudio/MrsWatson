//
// ErrorReporter.h - MrsWatson
// Created by Nik Reiman on 9/22/12.
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

#ifndef MrsWatson_ErrorReporter_h
#define MrsWatson_ErrorReporter_h

#include "base/CharString.h"
#include "plugin/PluginChain.h"


typedef struct {
  CharString reportName;
  CharString reportDirPath;
  CharString desktopPath;
  boolByte started;
  boolByte completed;
} ErrorReporterMembers;
typedef ErrorReporterMembers *ErrorReporter;

/**
 * Create a new ErrorReporter
 * @return ErrorReporter object
 */
ErrorReporter newErrorReporter(void);

/**
 * Create the necessary resources used by the ErrorReporter. This is not done
 * during object construction because large files can be copied and interactive
 * input requested from the user.
 * @param self
 */
void errorReporterInitialize(ErrorReporter self);

/**
 * Create a shell script that can be used to launch MrsWatson with the same
 * arguments which caused the error to happen
 * @param self
 * @param argc Number of arguments, as taken from main()
 * @param argv Argument array, as taken from main()
 */
void errorReporterCreateLauncher(ErrorReporter self, int argc, char *argv[]);

/**
 * Remap a resource to point to the ErrorReporter's directory. This ensures all
 * resources are contained within the same folder, and can be easily compressed
 * for sending after the program finishes executing.
 * @param self
 * @param path Input path to be remapped
 */
void errorReporterRemapPath(ErrorReporter self, CharString path);

/**
 * Copy a file to the ErrorReporter's directory
 * @param self
 * @param path File to copy
 * @return True if the file successfully copied
 */
boolByte errorReportCopyFileToReport(ErrorReporter self, CharString path);

/**
 * Ask the user if plugins should be copied to the report directory in addition
 * to other files. Because there can be copy protection issues, not to mention
 * file sizes, it is better to ask the user before copying the plugins.
 * @return True if the user wishes to copy all plugins
 */
boolByte errorReporterShouldCopyPlugins(void);

/**
 * Copy all plug-ins to the ErrorReporter directory
 * @param self
 * @param pluginChain Initialized plugin chain
 * @return True if all plugins were copied
 */
boolByte errorReporterCopyPlugins(ErrorReporter self, PluginChain pluginChain);

/**
 * Close any resources associated with the ErrorReporter
 * @param self
 */
void errorReporterClose(ErrorReporter self);

/**
 * Free all memory associated with an ErrorReporter instance
 * @param self
 */
void freeErrorReporter(ErrorReporter self);

#endif
