cmsUInt32Number GenerateCSA(cmsContext ContextID,
                            cmsHPROFILE hProfile,
                            cmsUInt32Number Intent,
                            cmsUInt32Number dwFlags,
                            cmsIOHANDLER* mem)
{
    cmsUInt32Number dwBytesUsed;
    cmsPipeline* lut = NULL;
    cmsStage* Matrix, *Shaper;


    // Is a named color profile?
    if (cmsGetDeviceClass(hProfile) == cmsSigNamedColorClass) {

        if (!WriteNamedColorCSA(mem, hProfile, Intent)) goto Error;
    }
    else {


        // Any profile class are allowed (including devicelink), but
        // output (PCS) colorspace must be XYZ or Lab
        cmsColorSpaceSignature ColorSpace = cmsGetPCS(hProfile);

        if (ColorSpace != cmsSigXYZData &&
            ColorSpace != cmsSigLabData) {

                cmsSignalError(ContextID, cmsERROR_COLORSPACE_CHECK, "Invalid output color space");
                goto Error;
        }


        // Read the lut with all necessary conversion stages
        lut = _cmsReadInputLUT(hProfile, Intent);
        if (lut == NULL) goto Error;


        // Tone curves + matrix can be implemented without any LUT
        if (cmsPipelineCheckAndRetreiveStages(lut, 2, cmsSigCurveSetElemType, cmsSigMatrixElemType, &Shaper, &Matrix)) {

            if (!WriteInputMatrixShaper(mem, hProfile, Matrix, Shaper)) goto Error;

        }
        else {
           // We need a LUT for the rest
           if (!WriteInputLUT(mem, hProfile, Intent, dwFlags)) goto Error;
        }
    }


    // Done, keep memory usage
    dwBytesUsed = mem ->UsedSpace;

    // Get rid of LUT
    if (lut != NULL) cmsPipelineFree(lut);

    // Finally, return used byte count
    return dwBytesUsed;

Error:
    if (lut != NULL) cmsPipelineFree(lut);
    return 0;
}