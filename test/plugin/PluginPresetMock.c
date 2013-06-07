#include "PluginPresetMock.h"

static boolByte _openPluginPresetMock(void* pluginPresetPtr) {
  PluginPreset pluginPreset = (PluginPreset)pluginPresetPtr;
  PluginPresetMockData extraData = (PluginPresetMockData)pluginPreset->extraData;
  extraData->isOpen = true;
  return true;
}

static boolByte _loadPluginPresetMock(void* pluginPresetPtr, Plugin plugin) {
  PluginPreset pluginPreset = (PluginPreset)pluginPresetPtr;
  PluginPresetMockData extraData = (PluginPresetMockData)pluginPreset->extraData;
  extraData->isLoaded = true;
  return true;
}

static void _freePluginPresetMock(void* extraDataPtr) {
}

PluginPreset newPluginPresetMock(void) {
  PluginPreset pluginPreset = (PluginPreset)malloc(sizeof(PluginPresetMembers));
  PluginPresetMockData extraData = (PluginPresetMockData)malloc(sizeof(PluginPresetMockDataMembers));

  pluginPreset->presetType = PRESET_TYPE_INTERNAL_PROGRAM;
  pluginPreset->presetName = newCharString();
  pluginPreset->compatiblePluginTypes = 0;
  pluginPresetSetCompatibleWith(pluginPreset, PLUGIN_TYPE_INTERNAL);

  pluginPreset->openPreset = _openPluginPresetMock;
  pluginPreset->loadPreset = _loadPluginPresetMock;
  pluginPreset->freePresetData = _freePluginPresetMock;

  extraData->isOpen = false;
  extraData->isLoaded = false;
  pluginPreset->extraData = extraData;

  return pluginPreset;  
}
