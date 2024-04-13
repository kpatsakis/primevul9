cmsToneCurve* CMSEXPORT cmsBuildGamma(cmsContext ContextID, cmsFloat64Number Gamma)
{
    return cmsBuildParametricToneCurve(ContextID, 1, &Gamma);
}