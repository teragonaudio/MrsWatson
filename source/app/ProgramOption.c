//
// ProgramOption.c - MrsWatson
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app/ProgramOption.h"
#include "audio/AudioSettings.h"
#include "base/File.h"
#include "base/LinkedList.h"
#include "logging/EventLogger.h"

CharString _programOptionGetString(const ProgramOption self);
float _programOptionGetNumber(const ProgramOption self);

ProgramOption newProgramOption(void) {
  return newProgramOptionWithName(-1, EMPTY_STRING, EMPTY_STRING, false,
    kProgramOptionTypeNumber, kProgramOptionArgumentTypeInvalid);
}

ProgramOption newProgramOptionWithName(const int optionIndex, const char* name,
  const char* help, boolByte hasShortForm, ProgramOptionType type,
  ProgramOptionArgumentType argumentType) {
  ProgramOption option = (ProgramOption)malloc(sizeof(ProgramOptionMembers));

  option->index = optionIndex;
  option->name = newCharStringWithCString(name);
  option->help = newCharStringWithCString(help);
  option->hasShortForm = hasShortForm;
  option->hideInHelp = false;

  option->type = type;
  switch(type) {
    case kProgramOptionTypeEmpty:
      // Nothing needed here
      break;
    case kProgramOptionTypeString:
      option->_data.string = newCharString();
      break;
    case kProgramOptionTypeNumber:
      option->_data.number = 0.0f;
      break;
    case kProgramOptionTypeList:
      option->_data.list = newLinkedList();
      break;
    default:
      logInternalError("ProgramOption with invalid type");
      break;
  }
  option->argumentType = argumentType;
  option->enabled = false;

  return option;
}

void _programOptionPrintDefaultValue(const ProgramOption self) {
  CharString stringValue;

  switch(self->type) {
    case kProgramOptionTypeString:
      stringValue = _programOptionGetString(self);
      if(stringValue != NULL && !charStringIsEmpty(stringValue)) {
        printf(", default value '%s'", stringValue->data);
      }
      break;
    case kProgramOptionTypeNumber:
      printf(", default value: %.0f", _programOptionGetNumber(self));
      break;
    default:
      break;
  }
}

void programOptionPrintHelp(const ProgramOption self, boolByte withFullHelp, int indentSize, int initialIndent) {
  CharString wrappedHelpString;
  int i;

  if(self == NULL) {
    logError("Can't find help for that option. Try running with --help to see all options\n");
    return;
  }

  // Initial argument indent
  for(i = 0; i < initialIndent; i ++) {
    printf(" ");
  }

  // All arguments have a long form, so that will always be printed
  printf("--%s", self->name->data);

  if(self->hasShortForm) {
    printf(" (or -%c)", self->name->data[0]);
  }

  switch(self->argumentType) {
    case kProgramOptionArgumentTypeRequired:
      printf(" <argument>");
      break;
    case kProgramOptionArgumentTypeOptional:
      printf(" [argument]");
      break;
    case kProgramOptionArgumentTypeNone:
    default:
      break;
  }

  _programOptionPrintDefaultValue(self);

  if(withFullHelp) {
    // Newline and indentation before help
    wrappedHelpString = charStringWrap(self->help, initialIndent + indentSize);
    printf("\n%s\n\n", wrappedHelpString->data);
    freeCharString(wrappedHelpString);
  }
  else {
    printf("\n");
  }
}

CharString _programOptionGetString(const ProgramOption self) {
  return self->type == kProgramOptionTypeString ? self->_data.string : NULL;
}

float _programOptionGetNumber(const ProgramOption self) {
  return self->type == kProgramOptionTypeNumber ? self->_data.number : -1.0f;
}

static LinkedList _programOptionGetList(const ProgramOption self) {
  return self->type == kProgramOptionTypeList ? self->_data.list : NULL;
}

static void _programOptionSetString(ProgramOption self, const CharString value) {
  if(self->type == kProgramOptionTypeString) {
    charStringCopy(self->_data.string, value);
  }
}

static void _programOptionSetCString(ProgramOption self, const char* value) {
  CharString valueString = newCharStringWithCString(value);
  _programOptionSetString(self, valueString);
  freeCharString(valueString);
}

static void _programOptionSetNumber(ProgramOption self, const float value) {
  if(self->type == kProgramOptionTypeNumber) {
    self->_data.number = value;
  }
}

static void _programOptionSetListItem(ProgramOption self, void* value) {
  if(self->type == kProgramOptionTypeList) {
    linkedListAppend(self->_data.list, value);
  }
}

static void _programOptionSetData(ProgramOption self, const char* data) {
  if(data == NULL) {
    return;
  }

  switch(self->type) {
    case kProgramOptionTypeString:
      _programOptionSetCString(self, data);
      break;
    case kProgramOptionTypeNumber:
      // Windows doesn't do strtof :(
      _programOptionSetNumber(self, (float)strtod(data, NULL));
      break;
    case kProgramOptionTypeList:
      _programOptionSetListItem(self, (void*)data);
      break;
    default:
      logInternalError("Set ProgramOption with invalid type");
      break;
  }
}

void freeProgramOption(ProgramOption self) {
  if(self != NULL) {
    freeCharString(self->name);
    freeCharString(self->help);
    switch(self->type) {
      case kProgramOptionTypeString:
        freeCharString(self->_data.string);
        break;
      case kProgramOptionTypeList:
        // Note: This will not actually free the associated strings for this
        // option. This is ok if the list items are parsed from argv/argc, but
        // otherwise memory could be leaked here.
        freeLinkedList(self->_data.list);
        break;
      default:
        break;
    }
    free(self);
  }
}


ProgramOptions newProgramOptions(int numOptions) {
  ProgramOptions options = (ProgramOptions)malloc(sizeof(ProgramOptionsMembers));
  options->numOptions = numOptions;
  options->options = (ProgramOption*)malloc(sizeof(ProgramOption) * numOptions);
  memset(options->options, 0, sizeof(ProgramOption) * numOptions);
  return options;
}

boolByte programOptionsAdd(const ProgramOptions self, const ProgramOption option) {
  if(option != NULL && option->index < self->numOptions) {
    self->options[option->index] = option;
    return true;
  }
  return false;
}

static boolByte _isStringShortOption(const char* testString) {
  return (boolByte)(testString != NULL && strlen(testString) == 2 && testString[0] == '-');
}

static boolByte _isStringLongOption(const char* testString) {
  return (boolByte)(testString != NULL && strlen(testString) > 2 && testString[0] == '-' && testString[1] == '-');
}

static ProgramOption _findProgramOption(ProgramOptions self, const char* name) {
  ProgramOption potentialMatchOption, optionMatch;
  CharString optionStringWithoutDashes;
  unsigned int i;

  if(_isStringShortOption(name)) {
    for(i = 0; i < self->numOptions; i++) {
      potentialMatchOption = self->options[i];
      if(potentialMatchOption->hasShortForm && potentialMatchOption->name->data[0] == name[1]) {
        return potentialMatchOption;
      }
    }
  }

  if(_isStringLongOption(name)) {
    optionMatch = NULL;
    optionStringWithoutDashes = newCharStringWithCapacity(kCharStringLengthShort);
    strncpy(optionStringWithoutDashes->data, name + 2, strlen(name) - 2);
    for(i = 0; i < self->numOptions; i++) {
      potentialMatchOption = self->options[i];
      if(charStringIsEqualTo(potentialMatchOption->name, optionStringWithoutDashes, false)) {
        optionMatch = potentialMatchOption;
        break;
      }
    }
    freeCharString(optionStringWithoutDashes);
    return optionMatch;
  }

  // If no option was found, then return null
  return NULL;
}

static boolByte _fillOptionArgument(ProgramOption self, int* currentArgc, int argc, char** argv) {
  if(self->argumentType == kProgramOptionArgumentTypeNone) {
    return true;
  }
  else if(self->argumentType == kProgramOptionArgumentTypeOptional) {
    int potentialNextArgc = *currentArgc + 1;
    if(potentialNextArgc >= argc) {
      return true;
    }
    else {
      char* potentialNextArg = argv[potentialNextArgc];
      // If the next string in the sequence is NOT an argument, we assume it is the optional argument
      if(!_isStringShortOption(potentialNextArg) && !_isStringLongOption(potentialNextArg)) {
        _programOptionSetData(self, potentialNextArg);
        (*currentArgc)++;
        return true;
      }
      else {
        // Otherwise, it is another option, but that's ok
        return true;
      }
    }
  }
  else if(self->argumentType == kProgramOptionArgumentTypeRequired) {
    int nextArgc = *currentArgc + 1;
    if(nextArgc >= argc) {
      logCritical("Option '%s' requires an argument, but none was given", self->name->data);
      return false;
    }
    else {
      char* nextArg = argv[nextArgc];
      if(_isStringShortOption(nextArg) || _isStringLongOption(nextArg)) {
        logCritical("Option '%s' requires an argument, but '%s' is not valid", self->name->data, nextArg);
        return false;
      }
      else {
        _programOptionSetData(self, nextArg);
        (*currentArgc)++;
        return true;
      }
    }
  }
  else {
    logInternalError("Unknown argument type '%d'", self->argumentType);
    return false;
  }
}

boolByte programOptionsParseArgs(ProgramOptions self, int argc, char** argv) {
  int argumentIndex;
  for(argumentIndex = 1; argumentIndex < argc; argumentIndex++) {
    const ProgramOption option = _findProgramOption(self, argv[argumentIndex]);
    if(option == NULL) {
      logCritical("Invalid option '%s'", argv[argumentIndex]);
      return false;
    }
    else {
      if(_fillOptionArgument(option, &argumentIndex, argc, argv)) {
        option->enabled = true;
      }
      else {
        return false;
      }
    }
  }

  // If we make it to here, return true
  return true;
}

boolByte programOptionsParseConfigFile(ProgramOptions self, const CharString filename) {
  boolByte result = false;
  File configFile = NULL;
  LinkedList configFileLines = NULL;
  CharString* argvCharStrings;
  int argc;
  char** argv;
  int i;

  if(filename == NULL || charStringIsEmpty(filename)) {
    logCritical("Cannot read options from empty filename");
    return false;
  }

  configFile = newFileWithPath(filename);
  if(configFile == NULL || configFile->fileType != kFileTypeFile) {
    logCritical("Cannot read options from non-existent file '%s'", filename->data);
    freeFile(configFile);
    return false;
  }

  configFileLines = fileReadLines(configFile);
  if(configFileLines == NULL) {
    logInternalError("Could not split config file lines");
    return false;
  }
  else if(linkedListLength(configFileLines) == 0) {
    logInfo("Config file '%s' is empty", filename->data);
    freeLinkedList(configFileLines);
    freeFile(configFile);
    return true;
  }
  else {
    // Don't need the file anymore, it can be freed here
    freeFile(configFile);
  }

  argvCharStrings = (CharString*)linkedListToArray(configFileLines);
  argc = linkedListLength(configFileLines);
  argv = (char**)malloc(sizeof(char*) * (argc + 1));
  // Normally this would be the application name, don't care about it here
  argv[0] = NULL;
  for(i = 0; i < argc; i++) {
    argv[i + 1] = argvCharStrings[i]->data;
  }
  argc++;
  result = programOptionsParseArgs(self, argc, argv);

  freeLinkedListAndItems(configFileLines, (LinkedListFreeItemFunc)freeCharString);
  free(argvCharStrings);
  free(argv);
  return result;
}

void programOptionsPrintHelp(const ProgramOptions self, boolByte withFullHelp, int indentSize) {
  unsigned int i;
  for(i = 0; i < self->numOptions; i++) {
    if(!self->options[i]->hideInHelp) {
      programOptionPrintHelp(self->options[i], withFullHelp, indentSize, indentSize);
    }
  }
}

ProgramOption programOptionsFind(const ProgramOptions self, const CharString string) {
  unsigned int i;
  for(i = 0; i < self->numOptions; i++) {
    if(charStringIsEqualTo(string, self->options[i]->name, true)) {
      return self->options[i];
    }
  }
  return NULL;
}

void programOptionsPrintHelpForOption(const ProgramOptions self, const CharString string,
  boolByte withFullHelp, int indentSize) {
  programOptionPrintHelp(programOptionsFind(self, string), withFullHelp, indentSize, 0);
}

const CharString programOptionsGetString(const ProgramOptions self, const unsigned int index) {
  return index < self->numOptions ? _programOptionGetString(self->options[index]) : NULL;
}

float programOptionsGetNumber(const ProgramOptions self, const unsigned int index) {
  return index < self->numOptions ? _programOptionGetNumber(self->options[index]) : -1.0f;
}

const LinkedList programOptionsGetList(const ProgramOptions self, const unsigned int index) {
  return index < self->numOptions ? _programOptionGetList(self->options[index]) : NULL;
}

void programOptionsSetCString(ProgramOptions self, const unsigned int index, const char* value) {
  if(index < self->numOptions) {
    _programOptionSetCString(self->options[index], value);
  }
}

void programOptionsSetString(ProgramOptions self, const unsigned int index, const CharString value) {
  if(index < self->numOptions) {
    _programOptionSetString(self->options[index], value);
  }
}

void programOptionsSetNumber(ProgramOptions self, const unsigned int index, const float value) {
  if(index < self->numOptions) {
    _programOptionSetNumber(self->options[index], value);
  }
}

void programOptionsSetListItem(ProgramOptions self, const unsigned int index, void* value) {
  if(index < self->numOptions) {
    _programOptionSetListItem(self->options[index], value);
  }
}

void freeProgramOptions(ProgramOptions self) {
  unsigned int i;

  if(self == NULL) {
    return;
  }
  for(i = 0; i < self->numOptions; i++) {
    freeProgramOption(self->options[i]);
  }
  free(self->options);
  free(self);
}
