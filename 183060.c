cmsToneCurve* ExtractGray2Y(cmsContext ContextID, cmsHPROFILE hProfile, cmsUInt32Number Intent)
{
    cmsToneCurve* Out = cmsBuildTabulatedToneCurve16(ContextID, 256, NULL);
    cmsHPROFILE hXYZ  = cmsCreateXYZProfile();
    cmsHTRANSFORM xform = cmsCreateTransformTHR(ContextID, hProfile, TYPE_GRAY_8, hXYZ, TYPE_XYZ_DBL, Intent, cmsFLAGS_NOOPTIMIZE);
    int i;

    if (Out != NULL && xform != NULL) {
        for (i=0; i < 256; i++) {

            cmsUInt8Number Gray = (cmsUInt8Number) i;
            cmsCIEXYZ XYZ;

            cmsDoTransform(xform, &Gray, &XYZ, 1);

            Out ->Table16[i] =_cmsQuickSaturateWord(XYZ.Y * 65535.0);
        }
    }

    if (xform) cmsDeleteTransform(xform);
    if (hXYZ) cmsCloseProfile(hXYZ);
    return Out;
}