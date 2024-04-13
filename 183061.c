int WriteInputLUT(cmsIOHANDLER* m, cmsHPROFILE hProfile, cmsUInt32Number Intent, cmsUInt32Number dwFlags)
{
    cmsHPROFILE hLab;
    cmsHTRANSFORM xform;
    cmsUInt32Number nChannels;
    cmsUInt32Number InputFormat;
    int rc;
    cmsHPROFILE Profiles[2];
    cmsCIEXYZ BlackPointAdaptedToD50;

    // Does create a device-link based transform.
    // The DeviceLink is next dumped as working CSA.

    InputFormat = cmsFormatterForColorspaceOfProfile(hProfile, 2, FALSE);
    nChannels   = T_CHANNELS(InputFormat);


    cmsDetectBlackPoint(&BlackPointAdaptedToD50, hProfile, Intent, 0);

    // Adjust output to Lab4
    hLab = cmsCreateLab4ProfileTHR(m ->ContextID, NULL);

    Profiles[0] = hProfile;
    Profiles[1] = hLab;

    xform = cmsCreateMultiprofileTransform(Profiles, 2,  InputFormat, TYPE_Lab_DBL, Intent, 0);
    cmsCloseProfile(hLab);

    if (xform == NULL) {

        cmsSignalError(m ->ContextID, cmsERROR_COLORSPACE_CHECK, "Cannot create transform Profile -> Lab");
        return 0;
    }

    // Only 1, 3 and 4 channels are allowed

    switch (nChannels) {

    case 1: {
            cmsToneCurve* Gray2Y = ExtractGray2Y(m ->ContextID, hProfile, Intent);
            EmitCIEBasedA(m, Gray2Y, &BlackPointAdaptedToD50);
            cmsFreeToneCurve(Gray2Y);
            }
            break;

    case 3:
    case 4: {
            cmsUInt32Number OutFrm = TYPE_Lab_16;
            cmsPipeline* DeviceLink;
            _cmsTRANSFORM* v = (_cmsTRANSFORM*) xform;

            DeviceLink = cmsPipelineDup(v ->Lut);
            if (DeviceLink == NULL) return 0;

            dwFlags |= cmsFLAGS_FORCE_CLUT;
            _cmsOptimizePipeline(m->ContextID, &DeviceLink, Intent, &InputFormat, &OutFrm, &dwFlags);

            rc = EmitCIEBasedDEF(m, DeviceLink, Intent, &BlackPointAdaptedToD50);
            cmsPipelineFree(DeviceLink);
            if (rc == 0) return 0;
            }
            break;

    default:

        cmsSignalError(m ->ContextID, cmsERROR_COLORSPACE_CHECK, "Only 3, 4 channels supported for CSA. This profile has %d channels.", nChannels);
        return 0;
    }


    cmsDeleteTransform(xform);

    return 1;
}