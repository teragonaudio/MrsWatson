- Determine target python version
  ^ I'm leaning towards python-2.6
- Figure out distribution under different platforms
- Same goes with installation
  ^ I don't want to use easy_install if at all possible
- Set up virtual python environment (with virtualenv)
- See if python modules already exist for the following functions, if
  not, then we will need to code these modules by hand:
  * Loading VST plugins into python for processing
  * Decoding WAV/AIFF data to raw PCM
  * Decoding compressed (mp3/ogg) data to raw PCM
    ^ This feature is not a high priority
  * Encoding raw PCM data to WAV/AIFF
  * Encoding raw PCM data to mp3/ogg
    ^ This feature is definitely not a high priority; many command line
    tools like lame already exist for doing this
  * Reading MIDI events from file
