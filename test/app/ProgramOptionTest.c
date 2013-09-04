#include "unit/TestRunner.h"
#include "app/ProgramOption.h"
#include "base/File.h"
#include "base/Types.h"

#if UNIX
#define TEST_CONFIG_FILE "/tmp/mrswatsontest-config.txt"
#elif WINDOWS
#define TEST_CONFIG_FILE "C:\\Temp\\mrswatsontest-config.txt"
#else
#define TEST_CONFIG_FILE "mrswatsontest-config.txt"
#endif

static void _programOptionTeardown(void) {
  CharString configFilePath = newCharStringWithCString(TEST_CONFIG_FILE);
  File configFile = newFileWithPath(configFilePath);
  if(fileExists(configFile)) {
    fileRemove(configFile);
  }
  freeCharString(configFilePath);
  freeFile(configFile);
}

static ProgramOption _getTestOption(void) {
  return newProgramOptionWithName(0, "test", "test help", true,
    kProgramOptionTypeNumber, kProgramOptionArgumentTypeOptional);
}

static int _testNewProgramOptions(void) {
  ProgramOptions p = newProgramOptions(4);
  assertIntEquals(p->numOptions, 4);
  freeProgramOptions(p);
  return 0;
}

static int _testAddNewProgramOption(void) {
  ProgramOptions p = newProgramOptions(1);
  ProgramOption o;
  assert(programOptionsAdd(p, _getTestOption()));
  assertIntEquals(p->numOptions, 1);
  o = p->options[0];
  assertNotNull(o);
  assertCharStringEquals(o->name, "test");
  freeProgramOptions(p);
  return 0;
}

static int _testAddNullProgramOption(void) {
  ProgramOptions p = newProgramOptions(1);
  assertFalse(programOptionsAdd(p, NULL));
  freeProgramOptions(p);
  return 0;
}

static int _testAddNewProgramOptionOutsideRange(void) {
  ProgramOptions p = newProgramOptions(1);
  ProgramOption o = _getTestOption();
  o->index++;
  assertFalse(programOptionsAdd(p, o));
  assertIntEquals(p->numOptions, 1);
  freeProgramOption(o);
  freeProgramOptions(p);
  return 0;
}

static int _testParseCommandLineShortOption(void) {
  ProgramOptions p = newProgramOptions(1);
  char* argv[2];
  argv[0] = "exe";
  argv[1] = "-t";
  assert(programOptionsAdd(p, _getTestOption()));
  assertFalse(p->options[0]->enabled);
  assert(programOptionsParseArgs(p, 2, argv));
  assert(p->options[0]->enabled);
  freeProgramOptions(p);
  return 0;
}

static int _testParseCommandLineLongOption(void) {
  ProgramOptions p = newProgramOptions(1);
  char* argv[2];
  argv[0] = "exe";
  argv[1] = "--test";
  assert(programOptionsAdd(p, _getTestOption()));
  assertFalse(p->options[0]->enabled);
  assert(programOptionsParseArgs(p, 2, argv));
  assert(p->options[0]->enabled);
  freeProgramOptions(p);
  return 0;
}

static int _testParseCommandLineInvalidOption(void) {
  ProgramOptions p = newProgramOptions(1);
  char* argv[2];
  argv[0] = "exe";
  argv[1] = "invalid";
  assert(programOptionsAdd(p, _getTestOption()));
  assertFalse(p->options[0]->enabled);
  assertFalse(programOptionsParseArgs(p, 2, argv));
  assertFalse(p->options[0]->enabled);
  freeProgramOptions(p);
  return 0;
}

static int _testParseCommandLineRequiredOption(void) {
  ProgramOptions p = newProgramOptions(1);
  ProgramOption o = _getTestOption();
  char* argv[3];
  argv[0] = "exe";
  argv[1] = "--test";
  argv[2] = "1.23";
  o->argumentType = kProgramOptionArgumentTypeRequired;
  assert(programOptionsAdd(p, o));

  assertFalse(p->options[0]->enabled);
  assertDoubleEquals(programOptionsGetNumber(p, 0), 0.0f, 0.0f);
  assert(programOptionsParseArgs(p, 3, argv));
  assertDoubleEquals(programOptionsGetNumber(p, 0), 123.0f, 0.0f);
  assert(p->options[0]->enabled);

  freeProgramOptions(p);
  return 0;
}

static int _testParseCommandLineRequiredOptionMissing(void) {
  ProgramOptions p = newProgramOptions(1);
  ProgramOption o = _getTestOption();
  char* argv[2];
  argv[0] = "exe";
  argv[1] = "--test";
  o->argumentType = kProgramOptionArgumentTypeRequired;
  assert(programOptionsAdd(p, o));

  assertFalse(p->options[0]->enabled);
  assertDoubleEquals(programOptionsGetNumber(p, 0), 0.0f, 0.0f);
  assertFalse(programOptionsParseArgs(p, 2, argv));
  assertFalse(p->options[0]->enabled);

  freeProgramOptions(p);
  return 0;
}

static ProgramOptions _getTestProgramOptionsForConfigFile(void) {
  ProgramOptions p = newProgramOptions(2);
  ProgramOption o1, o2;
  o1 = newProgramOptionWithName(0, "test", "test help", true, kProgramOptionTypeString, kProgramOptionArgumentTypeNone);
  programOptionsAdd(p, o1);
  o2 = newProgramOptionWithName(1, "test2", "test help", true, kProgramOptionTypeString, kProgramOptionArgumentTypeRequired);
  programOptionsAdd(p, o2);
  return p;
}

static FILE* _openTestProgramConfigFile(void) {
  FILE* fp = fopen(TEST_CONFIG_FILE, "w");
  return fp;
}

static int _testParseConfigFile(void) {
  ProgramOptions p = _getTestProgramOptionsForConfigFile();
  CharString filename = newCharStringWithCString(TEST_CONFIG_FILE);
  FILE* fp = _openTestProgramConfigFile();
  fprintf(fp, "--test\n-s\nfoo\n");
  fclose(fp);
  assert(programOptionsParseConfigFile(p, filename));
  assert(p->options[0]->enabled);
  assert(p->options[1]->enabled);
  assertCharStringEquals(programOptionsGetString(p, 1), "foo");

  unlink(TEST_CONFIG_FILE);
  freeProgramOptions(p);
  freeCharString(filename);
  return 0;
}

static int _testParseInvalidConfigFile(void) {
  ProgramOptions p = newProgramOptions(1);
  CharString filename = newCharStringWithCString("invalid");
  assertFalse(programOptionsParseConfigFile(p, filename));
  freeProgramOptions(p);
  freeCharString(filename);
  return 0;
}

static int _testParseNullConfigFile(void) {
  ProgramOptions p = newProgramOptions(1);
  assertFalse(programOptionsParseConfigFile(p, NULL));
  freeProgramOptions(p);
  return 0;
}

static int _testParseConfigFileWithInvalidOptions(void) {
  ProgramOptions p = _getTestProgramOptionsForConfigFile();
  CharString filename = newCharStringWithCString(TEST_CONFIG_FILE);
  FILE* fp = _openTestProgramConfigFile();

  fprintf(fp, "--test\n-s\n");
  fclose(fp);
  assertFalse(programOptionsParseConfigFile(p, filename));
  assert(p->options[0]->enabled);
  assertFalse(p->options[1]->enabled);

  unlink(TEST_CONFIG_FILE);
  freeProgramOptions(p);
  freeCharString(filename);
  return 0;
}

static int _testFindProgramOptionFromString(void) {
  ProgramOptions p = newProgramOptions(1);
  CharString c = newCharStringWithCString("test");
  ProgramOption o;

  assert(programOptionsAdd(p, _getTestOption()));
  assertIntEquals(p->numOptions, 1);
  o = programOptionsFind(p, c);
  assertNotNull(o);
  assertCharStringEquals(o->name, "test");

  freeProgramOptions(p);
  freeCharString(c);
  return 0;
}

static int _testFindProgramOptionFromStringInvalid(void) {
  ProgramOptions p = newProgramOptions(1);
  CharString c = newCharStringWithCString("invalid");
  ProgramOption o;

  assert(programOptionsAdd(p, _getTestOption()));
  assertIntEquals(p->numOptions, 1);
  o = programOptionsFind(p, c);
  assertIsNull(o);

  freeProgramOptions(p);
  freeCharString(c);
  return 0;
}

TestSuite addProgramOptionTests(void);
TestSuite addProgramOptionTests(void) {
  TestSuite testSuite = newTestSuite("ProgramOption", NULL, _programOptionTeardown);
  addTest(testSuite, "NewObject", _testNewProgramOptions);
  addTest(testSuite, "AddNewProgramOption", _testAddNewProgramOption);
  addTest(testSuite, "AddNullProgramOption", _testAddNullProgramOption);
  addTest(testSuite, "AddNewProgramOptionOutsideRange", _testAddNewProgramOptionOutsideRange);

  addTest(testSuite, "ParseCommandLineShortOption", _testParseCommandLineShortOption);
  addTest(testSuite, "ParseCommandLineLongOption", _testParseCommandLineLongOption);
  addTest(testSuite, "ParseCommandLineInvalidOption", _testParseCommandLineInvalidOption);
  addTest(testSuite, "ParseCommandLineRequiredOption", _testParseCommandLineRequiredOption);
  addTest(testSuite, "ParseCommandLineRequiredOptionMissing", _testParseCommandLineRequiredOptionMissing);

  addTest(testSuite, "ParseConfigFile", _testParseConfigFile);
  addTest(testSuite, "ParseInvalidConfigFile", _testParseInvalidConfigFile);
  addTest(testSuite, "ParseNullConfigFile", _testParseNullConfigFile);
  addTest(testSuite, "ParseConfigFileWithInvalidOptions", _testParseConfigFileWithInvalidOptions);

  addTest(testSuite, "FindProgramOptionFromString", _testFindProgramOptionFromString);
  addTest(testSuite, "FindProgramOptionFromStringInvalid", _testFindProgramOptionFromStringInvalid);
  return testSuite;
}
