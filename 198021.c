void SoundTouch::clear()
{
    samplesExpectedOut = 0;
    samplesOutput = 0;
    pRateTransposer->clear();
    pTDStretch->clear();
}