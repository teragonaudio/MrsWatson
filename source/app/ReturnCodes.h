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

// Exit result codes
typedef enum {
  RETURN_CODE_SUCCESS,
  RETURN_CODE_NOT_RUN,
  RETURN_CODE_INVALID_ARGUMENT,
  RETURN_CODE_MISSING_REQUIRED_OPTION,
  RETURN_CODE_IO_ERROR,
  RETURN_CODE_PLUGIN_ERROR,
  RETURN_CODE_INVALID_PLUGIN_CHAIN,
  RETURN_CODE_UNSUPPORTED_FEATURE,
  RETURN_CODE_INTERNAL_ERROR,
  // This return code should always be right before the last one. It is not
  // actually used, but instead we add the signal number to it and exit with
  // that code instead.
  RETURN_CODE_SIGNAL,
  NUM_RETURN_CODES
} ReturnCodes;

#endif
