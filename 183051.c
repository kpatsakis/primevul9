cmsUInt8Number Word2Byte(cmsUInt16Number w)
{
    return (cmsUInt8Number) floor((cmsFloat64Number) w / 257.0 + 0.5);
}