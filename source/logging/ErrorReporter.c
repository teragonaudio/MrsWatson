//
// ErrorReport.c - MrsWatson
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "app/BuildInfo.h"
#include "base/File.h"
#include "base/PlatformInfo.h"
#include "logging/ErrorReporter.h"
#include "logging/EventLogger.h"

#if WINDOWS
#include <Shlobj.h>
#endif

static const char *kErrorReportInfoText = "MrsWatson is now running \
in error report mode, which will generate a report on your desktop with \
any input/output sources and error logs. This also enables some extra \
arguments and will disable console logging.\n";
static const char *kErrorReportCopyPluginsPromptText = "Would you like to copy the \
plugin to the report? All data sent to the official support address is kept \
strictly confidential. If the plugin in question has copy protection (or is \
cracked), or depends on external resources, this probably won't work. But if \
the plugin can be copied, it greatly helps in fixing bugs.\n\
Copy the plugin? (y/n) ";


ErrorReporter newErrorReporter(void)
{
    ErrorReporter errorReporter = (ErrorReporter)malloc(sizeof(ErrorReporterMembers));

    errorReporter->started = false;
    errorReporter->completed = false;
    errorReporter->reportName = newCharString();

    errorReporter->desktopPath = newCharString();
    errorReporter->reportDirPath = newCharString();

    return errorReporter;
}

void errorReporterInitialize(ErrorReporter self)
{
    CharString infoText = newCharStringWithCString(kErrorReportInfoText);
    CharString wrappedInfoText;
    time_t now;
    size_t length;
    size_t i;

    printf("=== Starting error report ===\n");
    wrappedInfoText = charStringWrap(infoText, 0);
    // The second newline here is intentional
    printf("%s\n", wrappedInfoText->data);

    time(&now);
    self->started = true;

    snprintf(self->reportName->data, self->reportName->capacity,
             "MrsWatson Report %s", ctime(&now));
    // Trim the final newline character from this string if it exists
    length = strlen(self->reportName->data);

    if (self->reportName->data[length - 1] == '\n') {
        self->reportName->data[length - 1] = '\0';
        length--;
    }

    for (i = 0; i < length; i++) {
        if (!(charStringIsLetter(self->reportName, i) ||
                charStringIsNumber(self->reportName, i))) {
            self->reportName->data[i] = '-';
        }
    }

#if UNIX
    snprintf(self->desktopPath->data, self->desktopPath->capacity,
             "%s/Desktop", getenv("HOME"));
#elif WINDOWS
    SHGetFolderPathA(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, self->desktopPath->data);
#endif

    // Try to place the report on the user's desktop. However, if we cannot find
    // the desktop (which may occur with localized Linux installations, for instance),
    // then just dump it in the current directory instead.
    File desktopPath = newFileWithPath(self->desktopPath);
    File reportPath;

    if (!fileExists(desktopPath)) {
        logWarn("Could not find desktop location, placing error report in current directory instead");
        CharString currentDirString = fileGetCurrentDirectory();
        File currentDir = newFileWithPath(currentDirString);
        reportPath = newFileWithParent(currentDir, self->reportName);
        freeFile(currentDir);
        freeCharString(currentDirString);
    } else {
        reportPath = newFileWithParent(desktopPath, self->reportName);
        freeFile(desktopPath);
    }

    if (fileExists(reportPath)) {
        logCritical("The path '%s' already contains a previous error report. Please remove the report data and try again.");
    } else {
        fileCreate(reportPath, kFileTypeDirectory);
    }

    // Now we should have a real error report path
    charStringCopy(self->reportDirPath, reportPath->absolutePath);

    freeFile(reportPath);
    freeCharString(wrappedInfoText);
    freeCharString(infoText);
}

void errorReporterCreateLauncher(ErrorReporter self, int argc, char *argv[])
{
    CharString outScriptName = newCharString();
    FILE *scriptFilePointer;
    int i;

    charStringCopyCString(outScriptName, "run.sh");
    errorReporterRemapPath(self, outScriptName);
    scriptFilePointer = fopen(outScriptName->data, "w");
    freeCharString(outScriptName);
    fprintf(scriptFilePointer, "#!/bin/sh\n");
    fprintf(scriptFilePointer, "mrswatson");

    for (i = 1; i < argc; i++) {
        // Don't run with the error report option again
        if (strcmp(argv[i], "--error-report")) {
            fprintf(scriptFilePointer, " %s", argv[i]);
        }
    }

    fprintf(scriptFilePointer, "\n");
    fclose(scriptFilePointer);
}

void errorReporterRemapPath(ErrorReporter self, CharString path)
{
    File pathAsFile = newFileWithPath(path);
    CharString basename = fileGetBasename(pathAsFile);
    File parent = newFileWithPath(self->reportDirPath);
    File remappedPath = newFileWithParent(parent, basename);

    charStringCopy(path, remappedPath->absolutePath);

    freeCharString(basename);
    freeFile(parent);
    freeFile(pathAsFile);
    freeFile(remappedPath);
}

boolByte errorReportCopyFileToReport(ErrorReporter self, CharString path)
{
    boolByte success;

    // Copy the destination path so that the original is not modified
    CharString destination = newCharString();
    charStringCopy(destination, path);
    errorReporterRemapPath(self, destination);

    File reportDirPath = newFileWithPath(self->reportDirPath);
    File distinationPath = newFileWithPath(path);
    File result = fileCopyTo(distinationPath, reportDirPath);
    success = fileExists(result);

    freeCharString(destination);
    freeFile(reportDirPath);
    freeFile(distinationPath);
    freeFile(result);
    return success;
}

// This could live in File, however this is currently the only place it is being used
// and also it's a rather cheap hack, so I would prefer to keep it as a static function
// here until another use-case presents itself. If that should happen, then we should
// refactor this code properly and move it to File.
static boolByte _copyDirectoryToErrorReportDir(ErrorReporter self, CharString path)
{
    boolByte success;

#if UNIX
    int result;
    CharString copyCommand = newCharString();
    // TODO: This is the lazy way of doing this...
    snprintf(copyCommand->data, copyCommand->capacity, "/bin/cp -r \"%s\" \"%s\"",
             path->data, self->reportDirPath->data);
    result = system(copyCommand->data);
    success = (boolByte)(WEXITSTATUS(result) == 0);

    if (!success) {
        logError("Could not copy '%s' to '%s'\n", path->data, self->reportDirPath->data);
    }

#else
    logUnsupportedFeature("Copy directory recursively");
    success = false;
#endif

    return success;
}

boolByte errorReporterShouldCopyPlugins(void)
{
    CharString promptText = newCharStringWithCString(kErrorReportCopyPluginsPromptText);
    CharString wrappedPromptText;
    char response;

    wrappedPromptText = charStringWrap(promptText, 0);
    printf("%s", wrappedPromptText->data);
    freeCharString(wrappedPromptText);
    freeCharString(promptText);

    response = (char)getchar();
    return (boolByte)(response == 'y' || response == 'Y');
}

boolByte errorReporterCopyPlugins(ErrorReporter self, PluginChain pluginChain)
{
    CharString pluginAbsolutePath = NULL;
    Plugin currentPlugin = NULL;
    boolByte failed = false;
    unsigned int i;
    PlatformInfo platform = newPlatformInfo();

    for (i = 0; i < pluginChain->numPlugins; i++) {
        currentPlugin = pluginChain->plugins[i];
        pluginAbsolutePath = currentPlugin->pluginAbsolutePath;

        if (charStringIsEmpty(pluginAbsolutePath)) {
            logInfo("Plugin '%s' does not have an absolute path and could not be copied", currentPlugin->pluginName->data);
        } else if (platform->type == PLATFORM_MACOSX) {
            failed |= !_copyDirectoryToErrorReportDir(self, pluginAbsolutePath);
        } else {
            failed |= !errorReportCopyFileToReport(self, pluginAbsolutePath);
        }
    }

    freePlatformInfo(platform);
    return (boolByte)!failed;
}

void errorReporterClose(ErrorReporter self)
{
    // Always do this, just in case
    flushErrorLog();

    printf("\n=== Error report complete ===\n");
    printf("Created error report at %s\n", self->reportDirPath->data);
    printf("Please compress and email the report to: %s\n", SUPPORT_EMAIL);
    printf("Thanks!\n");
}

void freeErrorReporter(ErrorReporter errorReporter)
{
    if (errorReporter != NULL) {
        freeCharString(errorReporter->reportName);
        freeCharString(errorReporter->reportDirPath);
        freeCharString(errorReporter->desktopPath);
        free(errorReporter);
    }
}
