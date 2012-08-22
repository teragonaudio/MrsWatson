#include "ApplicationRunner.h"

static LinkedList _defaultArgs(void) {
  LinkedList args = newLinkedList();
  appendItemToList(args, "--quiet");
  return args;
}

static LinkedList _argsForRunWithNoArgs() {
  LinkedList args = _defaultArgs();
  return args;
}

void runMrsWatsonTests(char *applicationPath);
void runMrsWatsonTests(char *applicationPath) {
  runApplicationTest(applicationPath, "Run with no arguments", newLinkedList(), RETURN_CODE_NOT_RUN, NULL);
}
