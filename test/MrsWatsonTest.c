#include "ApplicationRunner.h"

static LinkedList _argsForScanPlugins(void) {
  LinkedList args = newLinkedList();
  appendItemToList(args, "--list-plugins");
  return args;
}

void runMrsWatsonTests(char *applicationPath);
void runMrsWatsonTests(char *applicationPath) {
  runApplicationTest(applicationPath, "Run with no plugins", newLinkedList(), RETURN_CODE_MISSING_REQUIRED_OPTION, NULL);
  runApplicationTest(applicationPath, "List plugins", _argsForScanPlugins(), RETURN_CODE_SUCCESS, NULL);
}
