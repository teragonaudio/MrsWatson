//
// base64.c - MrsWatson
// Created by Nik Reiman on 24 Jan 15.
// Copyright (c) 2015 Teragon Audio. All rights reserved.
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

#include "base64.h"
#include <stdlib.h>

// Adapted from http://stackoverflow.com/a/6782480/14302
static const char encodingTable[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
};
static int modTable[] = {0, 2, 1};


char *base64Encode(const unsigned char *data,
                   size_t inputSize,
                   size_t *outputSize)
{
    *outputSize = 4 * ((inputSize + 2) / 3);
    char *encodedData = (char *)malloc(*outputSize);
    if (encodedData == NULL) {
        return NULL;
    }

    for (size_t i = 0, j = 0; i < inputSize;) {
        unsigned int octet1 = i < inputSize ? data[i++] : 0;
        unsigned int octet2 = i < inputSize ? data[i++] : 0;
        unsigned int octet3 = i < inputSize ? data[i++] : 0;

        unsigned int triple = (octet1 << 0x10) + (octet2 << 0x08) + octet3;

        encodedData[j++] = encodingTable[(triple >> 3 * 6) & 0x3F];
        encodedData[j++] = encodingTable[(triple >> 2 * 6) & 0x3F];
        encodedData[j++] = encodingTable[(triple >> 1 * 6) & 0x3F];
        encodedData[j++] = encodingTable[(triple >> 0 * 6) & 0x3F];
    }

    for (size_t i = 0; i < modTable[inputSize % 3]; i++) {
        encodedData[*outputSize - 1 - i] = '=';
    }

    return encodedData;
}

unsigned char *base64Decode(const char *data,
                            size_t inputSize,
                            size_t *outputSize) {
    char decodingTable[256];
    for (char i = 0; i < 64; ++i) {
        decodingTable[(unsigned char)encodingTable[i]] = i;
    }

    if (inputSize % 4 != 0) {
        return NULL;
    }

    *outputSize = inputSize / 4 * 3;
    if (data[inputSize - 1] == '=') {
        (*outputSize)--;
    }
    if (data[inputSize - 2] == '=') {
        (*outputSize)--;
    }

    unsigned char *decodedData = malloc(*outputSize);
    if (decodedData == NULL) {
        return NULL;
    }

    for (size_t i = 0, j = 0; i < inputSize;) {
        unsigned int sextet1 = data[i] == '=' ? 0 & i++ : (unsigned int)decodingTable[data[i++]];
        unsigned int sextet2 = data[i] == '=' ? 0 & i++ : (unsigned int)decodingTable[data[i++]];
        unsigned int sextet3 = data[i] == '=' ? 0 & i++ : (unsigned int)decodingTable[data[i++]];
        unsigned int sextet4 = data[i] == '=' ? 0 & i++ : (unsigned int)decodingTable[data[i++]];

        unsigned int triple = (sextet1 << 3 * 6) +
                (sextet2 << 2 * 6) +
                (sextet3 << 1 * 6) +
                (sextet4 << 0 * 6);

        if (j < *outputSize) {
            decodedData[j++] = (unsigned char)((triple >> 2 * 8) & 0xFF);
        }
        if (j < *outputSize) {
            decodedData[j++] = (unsigned char)((triple >> 1 * 8) & 0xFF);
        }
        if (j < *outputSize) {
            decodedData[j++] = (unsigned char)((triple >> 0 * 8) & 0xFF);
        }
    }

    return decodedData;
}
