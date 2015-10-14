#include <stdio.h>
#include <stdarg.h>

#include "ApplicationRunner.h"
#include "base/File.h"
#include "base/PlatformInfo.h"
#include "analysis/AnalyzeFile.h"

static const char *kApplicationRunnerOutputFolder = "out";
static const int kApplicationRunnerWaitTimeoutInMs = 1000;

CharString buildTestArgumentString(const char *arguments, ...)
{
    CharString formattedArguments;
    va_list argumentList;
    va_start(argumentList, arguments);
    formattedArguments = newCharStringWithCapacity(kCharStringLengthLong);
    vsnprintf(formattedArguments->data, formattedArguments->capacity, arguments, argumentList);
    va_end(argumentList);
    return formattedArguments;
}

CharString getTestResourcePath(const CharString resourcesPath, const char *resourceType, const char *resourceName)
{
    CharString filename = newCharString();
    snprintf(filename->data, filename->capacity, "%s%c%s%c%s",
             resourcesPath->data, PATH_DELIMITER, resourceType, PATH_DELIMITER, resourceName);
    return filename;
}

CharString getTestOutputFilename(const char *testName, const TestOutputType outputType)
{
    char *fileExtension = NULL;

    switch (outputType) {
    case kTestOutputNone:
        return NULL;

    case kTestOutputAiff:
        fileExtension = "aiff";
        break;

    case kTestOutputFlac:
        fileExtension = "flac";
        break;

    case kTestOutputPcm:
        fileExtension = "pcm";
        break;

    case kTestOutputText:
        fileExtension = "txt";
        break;

    case kTestOutputWave:
        fileExtension = "wav";
        break;

    default:
        return NULL;
    }

    char *space;
    char *spacePtr;
    CharString filename = newCharString();
    snprintf(filename->data, filename->capacity, "%s%c%s.%s",
             kApplicationRunnerOutputFolder, PATH_DELIMITER, testName, fileExtension);
    spacePtr = filename->data;

    do {
        space = strchr(spacePtr + 1, ' ');

        if (space == NULL || (unsigned int)(space - filename->data) > strlen(filename->data)) {
            break;
        } else {
            *space = '-';
        }
    } while (true);

    return filename;
}

static CharString _getTestPluginResourcesPath(const CharString resourcesPath)
{
    CharString pluginRoot = newCharString();
    PlatformInfo platform = newPlatformInfo();
    snprintf(pluginRoot->data, pluginRoot->capacity, "%s%cvst%c%s",
             resourcesPath->data, PATH_DELIMITER, PATH_DELIMITER, platform->shortName->data);
    freePlatformInfo(platform);
    return pluginRoot;
}

static CharString _getDefaultArguments(const char *testName,
                                       const CharString resourcesPath,
                                       const CharString outputFilename)
{
    CharString outString = newCharStringWithCapacity(kCharStringLengthLong);
    CharString logfileName = getTestOutputFilename(testName, kTestOutputText);
    CharString pluginRootPath = _getTestPluginResourcesPath(resourcesPath);
    snprintf(outString->data, outString->capacity,
             "--log-file \"%s\" --verbose --plugin-root \"%s\"",
             logfileName->data, pluginRootPath->data);

    if (outputFilename != NULL) {
        charStringAppendCString(outString, " --output \"");
        charStringAppend(outString, outputFilename);
        charStringAppendCString(outString, "\"");
    }

    freeCharString(logfileName);
    freeCharString(pluginRootPath);
    return outString;
}

static void _removeOutputFile(const char *argument)
{
    CharString outputFilename = newCharStringWithCString(argument);
    File outputFile = newFileWithPath(outputFilename);

    if (fileExists(outputFile)) {
        fileRemove(outputFile);
    }

    freeCharString(outputFilename);
    freeFile(outputFile);
}

static void _removeOutputFiles(const char *testName)
{
    // Remove all possible output files generated during testing
    CharString outputFilename;

    outputFilename = getTestOutputFilename(testName, kTestOutputAiff);
    _removeOutputFile(outputFilename->data);
    freeCharString(outputFilename);
#if USE_FLAC
    outputFilename = getTestOutputFilename(testName, kTestOutputFlac);
    _removeOutputFile(outputFilename->data);
    freeCharString(outputFilename);
#endif
    outputFilename = getTestOutputFilename(testName, kTestOutputPcm);
    _removeOutputFile(outputFilename->data);
    freeCharString(outputFilename);
    outputFilename = getTestOutputFilename(testName, kTestOutputText);
    _removeOutputFile(outputFilename->data);
    freeCharString(outputFilename);
    outputFilename = getTestOutputFilename(testName, kTestOutputWave);
    _removeOutputFile(outputFilename->data);
    freeCharString(outputFilename);
}

static const char *_getResultCodeString(const int resultCode)
{
    switch (resultCode) {
    case RETURN_CODE_SUCCESS:
        return "Success";

    case RETURN_CODE_NOT_RUN:
        return "Not run";

    case RETURN_CODE_INVALID_ARGUMENT:
        return "Invalid argument";

    case RETURN_CODE_MISSING_REQUIRED_OPTION:
        return "Missing required option";

    case RETURN_CODE_IO_ERROR:
        return "I/O error";

    case RETURN_CODE_PLUGIN_ERROR:
        return "Plugin error";

    case RETURN_CODE_INVALID_PLUGIN_CHAIN:
        return "Invalid plugin chain";

    case RETURN_CODE_UNSUPPORTED_FEATURE:
        return "Unsupported feature";

    case RETURN_CODE_INTERNAL_ERROR:
        return "Internal error";

    case RETURN_CODE_SIGNAL:
        return "Caught signal";

    default:
        return "Unknown";
    }
}

int runIntegrationTest(const char *testName,
                       CharString testArguments,
                       ReturnCode expectedResultCode,
                       const TestOutputType testOutputType,
                       const CharString mrsWatsonExePath,
                       const CharString resourcesPath)
{
    int result = 0;
    int returnCode;
    ReturnCode resultCode;
    ChannelCount failedAnalysisChannel;
    SampleCount failedAnalysisFrame;

#if WINDOWS
    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION  processInfo;
#endif

    // Remove files from a previous test run
    File outputFolder = newFileWithPathCString(kApplicationRunnerOutputFolder);

    if (fileExists(outputFolder)) {
        _removeOutputFiles(testName);
    } else {
        fileCreate(outputFolder, kFileTypeDirectory);
    }

    freeFile(outputFolder);

    if (mrsWatsonExePath == NULL) {
        return -1;
    } else {
        File mrsWatsonExe = newFileWithPath(mrsWatsonExePath);
        boolByte mrsWatsonExeExists = fileExists(mrsWatsonExe);
        freeFile(mrsWatsonExe);

        if (!mrsWatsonExeExists) {
            freeCharString(testArguments);
            return -1;
        }
    }

    if (resourcesPath == NULL) {
        freeCharString(testArguments);
        return -1;
    } else {
        File resourcesFile = newFileWithPath(resourcesPath);
        boolByte resourcesExists = fileExists(resourcesFile);
        freeFile(resourcesFile);

        if (!resourcesExists) {
            freeCharString(testArguments);
            return -1;
        }
    }

    // Create the command line argument
    CharString arguments = newCharStringWithCapacity(kCharStringLengthLong);
    charStringAppendCString(arguments, "\"");
    charStringAppend(arguments, mrsWatsonExePath);
    charStringAppendCString(arguments, "\"");
    charStringAppendCString(arguments, " ");
    CharString defaultArguments;
    CharString outputFilename = getTestOutputFilename(testName, testOutputType);
    defaultArguments = _getDefaultArguments(testName, resourcesPath, outputFilename);
    charStringAppend(arguments, defaultArguments);
    freeCharString(defaultArguments);
    charStringAppendCString(arguments, " ");
    charStringAppend(arguments, testArguments);
    // Although testArguments is passed into this function (and hence, it would generally not take
    // ownership of it), in this case we free the arguments here to make writing the test cases
    // simpler and reduce the amount of boilerplate code.
    freeCharString(testArguments);

#if WINDOWS
    memset(&startupInfo, 0, sizeof(startupInfo));
    memset(&processInfo, 0, sizeof(processInfo));
    startupInfo.cb = sizeof(startupInfo);
    returnCode = CreateProcessA((LPCSTR)(mrsWatsonExePath->data), (LPSTR)(arguments->data),
                                0, 0, false, CREATE_DEFAULT_ERROR_MODE, 0, 0, &startupInfo, &processInfo);

    if (returnCode) {
        // TODO: Check return codes for these calls
        WaitForSingleObject(processInfo.hProcess, kApplicationRunnerWaitTimeoutInMs);
        GetExitCodeProcess(processInfo.hProcess, (LPDWORD)&resultCode);
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    } else {
        logCritical("Could not launch process, got error %s", stringForLastError(GetLastError()));
        return 1;
    }

#else
    returnCode = system(arguments->data);
    resultCode = (ReturnCode)WEXITSTATUS(returnCode);
#endif
    freeCharString(arguments);

    if (resultCode == RETURN_CODE_FORK_FAILED ||
            resultCode == RETURN_CODE_SHELL_FAILED ||
            resultCode == RETURN_CODE_LAUNCH_FAILED_OTHER) {
        logCritical("Could not launch shell, got return code %d\n\
Please check the executable path specified in the --mrswatson-path argument.",
                    resultCode);
        return 1;
    } else if (resultCode == expectedResultCode) {
        CharString failedAnalysisFunctionName = newCharString();
        if (testOutputType != kTestOutputNone) {
            if (analyzeFile(outputFilename->data, failedAnalysisFunctionName,
                            &failedAnalysisChannel, &failedAnalysisFrame)) {
                // TODO:
//                if (!testEnvironment->results->keepFiles) {
//                    _removeOutputFiles(testName);
//                }
                result = 0;
            } else {
                fprintf(stderr, "Audio analysis check for %s failed at frame %lu, channel %d. ",
                        failedAnalysisFunctionName->data, failedAnalysisFrame, failedAnalysisChannel);
                result = 1;
            }
        } else {
            result = 0;

            // TODO:
//            if (!testEnvironment->results->keepFiles) {
//                _removeOutputFiles(testName);
//            }
        }
        freeCharString(failedAnalysisFunctionName);
    } else {
        fprintf(stderr, "Expected result code %d (%s), got %d (%s). ",
                expectedResultCode, _getResultCodeString(expectedResultCode),
                resultCode, _getResultCodeString(resultCode));
        result = 1;
    }

    freeCharString(outputFilename);
    return result;
}
