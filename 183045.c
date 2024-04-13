int WriteInputMatrixShaper(cmsIOHANDLER* m, cmsHPROFILE hProfile, cmsStage* Matrix, cmsStage* Shaper)
{
    cmsColorSpaceSignature ColorSpace;
    int rc;
    cmsCIEXYZ BlackPointAdaptedToD50;

    ColorSpace = cmsGetColorSpace(hProfile);

    cmsDetectBlackPoint(&BlackPointAdaptedToD50, hProfile, INTENT_RELATIVE_COLORIMETRIC, 0);

    if (ColorSpace == cmsSigGrayData) {

        cmsToneCurve** ShaperCurve = _cmsStageGetPtrToCurveSet(Shaper);
        rc = EmitCIEBasedA(m, ShaperCurve[0], &BlackPointAdaptedToD50);

    }
    else
        if (ColorSpace == cmsSigRgbData) {

            cmsMAT3 Mat;
            int i, j;

            memmove(&Mat, GetPtrToMatrix(Matrix), sizeof(Mat));

            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    Mat.v[i].n[j] *= MAX_ENCODEABLE_XYZ;

            rc = EmitCIEBasedABC(m, (cmsFloat64Number *)&Mat,
                _cmsStageGetPtrToCurveSet(Shaper),
                &BlackPointAdaptedToD50);
        }
        else {

            cmsSignalError(m->ContextID, cmsERROR_COLORSPACE_CHECK, "Profile is not suitable for CSA. Unsupported colorspace.");
            return 0;
        }

    return rc;
}