int EntriesByGamma(cmsFloat64Number Gamma)
{
    if (fabs(Gamma - 1.0) < 0.001) return 2;
    return 4096;
}