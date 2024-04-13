void SoundTouch::setRate(double newRate)
{
    virtualRate = newRate;
    calcEffectiveRateAndTempo();
}