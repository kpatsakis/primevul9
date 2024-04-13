void SoundTouch::setPitchSemiTones(double newPitch)
{
    setPitchOctaves(newPitch / 12.0);
}