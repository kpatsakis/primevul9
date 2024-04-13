cmsBool GammaTableEquals(cmsUInt16Number* g1, cmsUInt16Number* g2, cmsUInt32Number nEntries)
{
    return memcmp(g1, g2, nEntries* sizeof(cmsUInt16Number)) == 0;
}