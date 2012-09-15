extern void runAudioClockTests(void);
extern void runAudioSettingsTests(void);
extern void runCharStringTests(void);
extern void runLinkedListTests(void);
extern void runMidiSequenceTests(void);

void runInternalTestSuite(void);
void runInternalTestSuite(void) {
  runAudioClockTests();
  runAudioSettingsTests();
  runCharStringTests();
  runLinkedListTests();
  runMidiSequenceTests();
}