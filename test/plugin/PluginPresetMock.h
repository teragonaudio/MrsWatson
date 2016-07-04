#ifndef MrsWaston_PluginPresetMock_h
#define MrsWaston_PluginPresetMock_h

#include "base/Types.h"
#include "plugin/PluginPreset.h"

typedef struct {
  boolByte isOpen;
  boolByte isLoaded;
} PluginPresetMockDataMembers;
typedef PluginPresetMockDataMembers *PluginPresetMockData;

PluginPreset newPluginPresetMock(void);

#endif
