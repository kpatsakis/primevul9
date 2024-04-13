void SoundTouch::setPitch(double newPitch)
{
    virtualPitch = newPitch;
    calcEffectiveRateAndTempo();
}