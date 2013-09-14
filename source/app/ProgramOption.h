//
// ProgramOption.h - MrsWatson
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

#ifndef MrsWatson_ProgramOption_h
#define MrsWatson_ProgramOption_h

#include "base/CharString.h"
#include "base/LinkedList.h"
#include "base/Types.h"

typedef enum {
  kProgramOptionTypeEmpty,
  kProgramOptionTypeString,
  kProgramOptionTypeNumber,
  kProgramOptionTypeList,
  kProgramOptionTypeNumTypes
} ProgramOptionType;

typedef enum {
  kProgramOptionArgumentTypeNone,
  kProgramOptionArgumentTypeOptional,
  kProgramOptionArgumentTypeRequired,
  kProgramOptionArgumentTypeInvalid
} ProgramOptionArgumentType;

typedef union {
  CharString string;
  float number;
  LinkedList list;
} ProgramOptionData;

typedef struct {
  unsigned int index;
  CharString name;
  CharString help;
  boolByte hasShortForm;
  // For "hidden" options which should not be printed out in the help output
  boolByte hideInHelp;

  ProgramOptionType type;
  ProgramOptionData _data;
  ProgramOptionArgumentType argumentType;
  boolByte enabled;
} ProgramOptionMembers;
typedef ProgramOptionMembers* ProgramOption;

typedef struct {
  ProgramOption* options;
  unsigned int numOptions;
} ProgramOptionsMembers;
typedef ProgramOptionsMembers* ProgramOptions;


/**
 * Create a new ProgramOption instance
 * @return An initialized ProgramOption
 */
ProgramOption newProgramOption(void);

/**
 * Create a new ProgramOption instance with some default values
 * @param opnionIndex Reference index for option (ie, from an enum)
 * @param name Full option name, hyphenated in the case of multiple words
 * @param help Full help string
 * @param hasShortForm True if the option should also be matched with the first letter
 * @param argumentType Expected argument type which can be passed to this option
 * @return 
 */
ProgramOption newProgramOptionWithName(const int optionIndex, const char* name,
  const char* help, boolByte hasShortForm, ProgramOptionType type,
  ProgramOptionArgumentType argumentType);

/**
 * Print out help for the option
 * @param self
 * @param withFullHelp Print the entire help or just the argument name and summary
 * @param indentSize Number of spaces to indent output
 * @param initialIndent Initial number of spaces to offset output
 */
void programOptionPrintHelp(const ProgramOption self, boolByte withFullHelp,
  int indentSize, int initialIndent);

/**
 * Free memory used by a ProgramOption instance
 * @param self
 */
void freeProgramOption(ProgramOption self);


/**
 * Create a new ProgramOptions container
 * @param numOptions Number of options to hold
 * @return An initialized ProgramOptions
 */
ProgramOptions newProgramOptions(int numOptions);

/**
 * Add a ProgramOption instance to the collection
 * @param self
 * @param option Option to add to the collection. Note that this option must have
 * its index set correctly, as the ProgramOptions options array is statically
 * allocated to a set size when the object is initialized.
 * @return True on success, false if option is null or has an invalid index
 */
boolByte programOptionsAdd(const ProgramOptions self, const ProgramOption option);

/**
 * Find a ProgramOption by name
 * @param self
 * @param name Name to search for (case insensitive)
 * @return Matching ProgramOption, NULL otherwise
 */
ProgramOption programOptionsFind(const ProgramOptions self, const CharString name);

/**
 * Parse a command line argument array.
 * @param self
 * @param argc Number of arguments (ie, from main(int argc, char** argv)
 * @param argv Argument array (ie, from main(int argc, char** argv)
 * @return False if an error occurred during parsing, such as a missing or invalid argument
 */
boolByte programOptionsParseArgs(ProgramOptions self, int argc, char** argv);

/**
 * Parse a configuration file to options. File should be plain text with one
 * argument per line
 * @param self
 * @param filename Filename to parse
 * @return True if all options were correctly parsed, false if there was an error
 * either opening the file or with the arguments themselves.
 */
boolByte programOptionsParseConfigFile(ProgramOptions self, const CharString filename);

/**
 * Print out help for all options
 * @param self
 * @param withFullHelp Include full help text, or just option summaries
 * @param indentSize Indent size to use for output
 */
void programOptionsPrintHelp(const ProgramOptions self, boolByte withFullHelp, int indentSize);

/**
 * Find an option and print out its help
 * @param self
 * @param string Option name to find
 * @param withFullHelp True if full help should be shown, otherwise just the
 * summary string.
 * @param indentSize Indent size to use for output
 */
void programOptionsPrintHelpForOption(const ProgramOptions self, const CharString string,
  boolByte withFullHelp, int indentSize);

/**
 * Get string value for an option
 * @param self
 * @param index Option index
 * @return Option value string, or NULL if this option is of a different type
 */
const CharString programOptionsGetString(const ProgramOptions self, const unsigned int index);

/**
 * Get numeric value for an option
 * @param self
 * @param index Option index
 * @return Option value string, or -1 if this option is of a different type
 */
float programOptionsGetNumber(const ProgramOptions self, const unsigned int index);

/**
 * Get linked list values for an option
 * @param self
 * @param index Option index
 * @return Option value string, or NULL if this option is of a different type
 */
const LinkedList programOptionsGetList(const ProgramOptions self, const unsigned int index);

/**
 * Set an option's string value. If setting the wrong type to the option, this
 * call does nothing.
 * @param self
 * @param index Option index
 * @param value Value to set
 */
void programOptionsSetCString(ProgramOptions self, const unsigned int index, const char* value);

/**
 * Set an option's string value. If setting the wrong type to the option, this
 * call does nothing.
 * @param self
 * @param index Option index
 * @param value Value to set
 */
void programOptionsSetString(ProgramOptions self, const unsigned int index, const CharString value);

/**
 * Set an option's numeric value. If setting the wrong type to the option, this
 * call does nothing.
 * @param self
 * @param index Option index
 * @param value Value to set
 */
void programOptionsSetNumber(ProgramOptions self, const unsigned int index, const float value);

/**
 * Add an item to an option's linked list. If this option has the wrong type,
 * then this call does nothing.
 * @param self
 * @param index Option index
 * @param value Value to add
 */
void programOptionsSetListItem(ProgramOptions self, const unsigned int index, void* value);

/**
 * Free memory used by a ProgramOptions array and all options in the collection
 * @param self
 */
void freeProgramOptions(ProgramOptions self);

#endif
