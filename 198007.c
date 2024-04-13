void SoundTouch::setPitchSemiTones(int newPitch)
{
    setPitchOctaves((double)newPitch / 12.0);
}