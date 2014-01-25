#ifndef MrsWatson_PluginMock_h
#define MrsWatson_PluginMock_h

#include "base/Types.h"
#include "plugin/Plugin.h"

static const int kPluginMockTailTime = 123;

typedef struct {
  boolByte isOpen;
  boolByte isPrepared;
  boolByte processAudioCalled;
  boolByte processMidiCalled;
} PluginMockDataMembers;
typedef PluginMockDataMembers* PluginMockData;

Plugin newPluginMock(void);

#endif
