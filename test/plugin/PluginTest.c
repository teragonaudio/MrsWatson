#include "unit/TestRunner.h"
#include "plugin/Plugin.h"

static int _testPluginFactory(void)
{
    CharString silence = newCharStringWithCString("mrs_silence");
    CharString pluginRoot = newCharString();
    Plugin p = pluginFactory(silence, pluginRoot);

    assertNotNull(p);
    assertIntEquals(PLUGIN_TYPE_INTERNAL, p->interfaceType);
    assertCharStringEquals(silence->data, p->pluginName);

    freeCharString(silence);
    freeCharString(pluginRoot);
    freePlugin(p);
    return 0;
}

static int _testPluginFactoryInvalidPlugin(void)
{
    CharString invalid = newCharStringWithCString("invalid");
    CharString pluginRoot = newCharString();
    Plugin p = pluginFactory(invalid, pluginRoot);

    assertIsNull(p);

    freeCharString(invalid);
    freeCharString(pluginRoot);
    freePlugin(p);
    return 0;
}

static int _testPluginFactoryNullPluginName(void)
{
    CharString pluginRoot = newCharString();
    Plugin p = pluginFactory(NULL, pluginRoot);

    assertIsNull(p);

    freeCharString(pluginRoot);
    freePlugin(p);
    return 0;
}

static int _testPluginFactoryEmptyPluginName(void)
{
    CharString invalid = newCharString();
    CharString pluginRoot = newCharString();
    Plugin p = pluginFactory(invalid, pluginRoot);

    assertIsNull(p);

    freeCharString(invalid);
    freeCharString(pluginRoot);
    freePlugin(p);
    return 0;
}

static int _testPluginFactoryNullRoot(void)
{
    CharString silence = newCharStringWithCString("mrs_silence");
    Plugin p = pluginFactory(silence, NULL);

    assertNotNull(p);
    assertIntEquals(PLUGIN_TYPE_INTERNAL, p->interfaceType);
    assertCharStringEquals(silence->data, p->pluginName);

    freeCharString(silence);
    freePlugin(p);
    return 0;
}

static int _testFreeNullPlugin(void)
{
    freePlugin(NULL);
    return 0;
}

TestSuite addPluginTests(void);
TestSuite addPluginTests(void)
{
    TestSuite testSuite = newTestSuite("Plugin", NULL, NULL);
    addTest(testSuite, "PluginFactory", _testPluginFactory);
    addTest(testSuite, "PluginFactoryInvalidPlugin", _testPluginFactoryInvalidPlugin);
    addTest(testSuite, "PluginFactoryNullPluginName", _testPluginFactoryNullPluginName);
    addTest(testSuite, "PluginFactoryEmptyPluginName", _testPluginFactoryEmptyPluginName);
    addTest(testSuite, "PluginFactoryNullRoot", _testPluginFactoryNullRoot);
    addTest(testSuite, "FreeNullPlugin", _testFreeNullPlugin);
    return testSuite;
}
