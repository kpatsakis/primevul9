void SoundTouch::setTempo(double newTempo)
{
    virtualTempo = newTempo;
    calcEffectiveRateAndTempo();
}