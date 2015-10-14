//
// ReturnCodes.h - MrsWatson
// Created by Nik Reiman on 10 May 13.
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

#ifndef MrsWatson_ReturnCodes_h
#define MrsWatson_ReturnCodes_h

typedef enum {
    /**
     * Failed to fork a process (used by the integration tests).
     */
    RETURN_CODE_FORK_FAILED = -1,

    /**
     * Failed to create a shell for a forked process (used by the integration tests).
     */
    RETURN_CODE_SHELL_FAILED = 127,

    /**
     * Unknown or other error occurred when launching a process (used by the integration tests).
     */
    RETURN_CODE_LAUNCH_FAILED_OTHER = 255,

    /**
     * The operation completed successfully.
     */
    RETURN_CODE_SUCCESS = 0,

    /**
     * The program executed successfully, but did not process any audio.
     */
    RETURN_CODE_NOT_RUN,

    /**
     * An invalid argument was passed to the program.
     */
    RETURN_CODE_INVALID_ARGUMENT,

    /**
     * An option required an extra argument, but none was given.
     */
    RETURN_CODE_MISSING_REQUIRED_OPTION,

    /**
     * An I/O error occurred during processing.
     */
    RETURN_CODE_IO_ERROR,

    /**
     * A plugin failed to initialize.
     */
    RETURN_CODE_PLUGIN_ERROR,

    /**
     * The plugin chain has an invalid configuration and cannot process audio.
     */
    RETURN_CODE_INVALID_PLUGIN_CHAIN,

    /**
     * A feature was requested which is either unimplemented, or was deprecated and removed.
     */
    RETURN_CODE_UNSUPPORTED_FEATURE,

    /**
     * An internal error occurred.
     */
    RETURN_CODE_INTERNAL_ERROR,

    /**
     * A signal was caught, forcing termination.
     *
     * This return code should always be last in this enum list. It is not actually used, but
     * instead the signal number which was caught is added to this value.
     */
    RETURN_CODE_SIGNAL,
} ReturnCode;

#endif
