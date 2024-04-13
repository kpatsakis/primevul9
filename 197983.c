void SoundTouch::setPitchOctaves(double newPitch)
{
    virtualPitch = exp(0.69314718056 * newPitch);
    calcEffectiveRateAndTempo();
}