void SoundTouch::setSampleRate(uint srate)
{
    // set sample rate, leave other tempo changer parameters as they are.
    pTDStretch->setParameters((int)srate);
    bSrateSet = true;
}