int WriteOutputLUT(cmsIOHANDLER* m, cmsHPROFILE hProfile, cmsUInt32Number Intent, cmsUInt32Number dwFlags)
{
    cmsHPROFILE hLab;
    cmsHTRANSFORM xform;
    cmsUInt32Number i, nChannels;
    cmsUInt32Number OutputFormat;
    _cmsTRANSFORM* v;
    cmsPipeline* DeviceLink;
    cmsHPROFILE Profiles[3];
    cmsCIEXYZ BlackPointAdaptedToD50;
    cmsBool lDoBPC = (cmsBool) (dwFlags & cmsFLAGS_BLACKPOINTCOMPENSATION);
    cmsBool lFixWhite = (cmsBool) !(dwFlags & cmsFLAGS_NOWHITEONWHITEFIXUP);
    cmsUInt32Number InFrm = TYPE_Lab_16;
    cmsUInt32Number RelativeEncodingIntent;
    cmsColorSpaceSignature ColorSpace;


    hLab = cmsCreateLab4ProfileTHR(m ->ContextID, NULL);
    if (hLab == NULL) return 0;

    OutputFormat = cmsFormatterForColorspaceOfProfile(hProfile, 2, FALSE);
    nChannels    = T_CHANNELS(OutputFormat);

    ColorSpace = cmsGetColorSpace(hProfile);

    // For absolute colorimetric, the LUT is encoded as relative in order to preserve precision.

    RelativeEncodingIntent = Intent;
    if (RelativeEncodingIntent == INTENT_ABSOLUTE_COLORIMETRIC)
        RelativeEncodingIntent = INTENT_RELATIVE_COLORIMETRIC;


    // Use V4 Lab always
    Profiles[0] = hLab;
    Profiles[1] = hProfile;

    xform = cmsCreateMultiprofileTransformTHR(m ->ContextID,
                                              Profiles, 2, TYPE_Lab_DBL,
                                              OutputFormat, RelativeEncodingIntent, 0);
    cmsCloseProfile(hLab);

    if (xform == NULL) {

        cmsSignalError(m ->ContextID, cmsERROR_COLORSPACE_CHECK, "Cannot create transform Lab -> Profile in CRD creation");
        return 0;
    }

    // Get a copy of the internal devicelink
    v = (_cmsTRANSFORM*) xform;
    DeviceLink = cmsPipelineDup(v ->Lut);
    if (DeviceLink == NULL) return 0;


    // We need a CLUT
    dwFlags |= cmsFLAGS_FORCE_CLUT;
    _cmsOptimizePipeline(m->ContextID, &DeviceLink, RelativeEncodingIntent, &InFrm, &OutputFormat, &dwFlags);

    _cmsIOPrintf(m, "<<\n");
    _cmsIOPrintf(m, "/ColorRenderingType 1\n");


    cmsDetectBlackPoint(&BlackPointAdaptedToD50, hProfile, Intent, 0);

    // Emit headers, etc.
    EmitWhiteBlackD50(m, &BlackPointAdaptedToD50);
    EmitPQRStage(m, hProfile, lDoBPC, Intent == INTENT_ABSOLUTE_COLORIMETRIC);
    EmitXYZ2Lab(m);


    // FIXUP: map Lab (100, 0, 0) to perfect white, because the particular encoding for Lab
    // does map a=b=0 not falling into any specific node. Since range a,b goes -128..127,
    // zero is slightly moved towards right, so assure next node (in L=100 slice) is mapped to
    // zero. This would sacrifice a bit of highlights, but failure to do so would cause
    // scum dot. Ouch.

    if (Intent == INTENT_ABSOLUTE_COLORIMETRIC)
            lFixWhite = FALSE;

    _cmsIOPrintf(m, "/RenderTable ");


    WriteCLUT(m, cmsPipelineGetPtrToFirstStage(DeviceLink), "<", ">\n", "", "", lFixWhite, ColorSpace);

    _cmsIOPrintf(m, " %d {} bind ", nChannels);

    for (i=1; i < nChannels; i++)
            _cmsIOPrintf(m, "dup ");

    _cmsIOPrintf(m, "]\n");


    EmitIntent(m, Intent);

    _cmsIOPrintf(m, ">>\n");

    if (!(dwFlags & cmsFLAGS_NODEFAULTRESOURCEDEF)) {

        _cmsIOPrintf(m, "/Current exch /ColorRendering defineresource pop\n");
    }

    cmsPipelineFree(DeviceLink);
    cmsDeleteTransform(xform);

    return 1;
}