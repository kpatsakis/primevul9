SoundTouch::SoundTouch()
{
    // Initialize rate transposer and tempo changer instances

    pRateTransposer = new RateTransposer();
    pTDStretch = TDStretch::newInstance();

    setOutPipe(pTDStretch);

    rate = tempo = 0;

    virtualPitch = 
    virtualRate = 
    virtualTempo = 1.0;

    calcEffectiveRateAndTempo();

    samplesExpectedOut = 0;
    samplesOutput = 0;

    channels = 0;
    bSrateSet = false;
}