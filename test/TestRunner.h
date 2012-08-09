#include <stdio.h>

#define FAIL() printf("Failure in %s(), line %d\n", __func__, __LINE__)
#define _assert(condition) do { if(!(condition)) { FAIL(); return 1; } } while(0)
#define _runTest(test) do { int result = test(); testsRun++; if(result) { return result; } } while(0)

static int testsRun = 0;
