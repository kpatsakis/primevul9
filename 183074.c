cmsUInt32Number CMSEXPORT cmsGetPostScriptColorResource(cmsContext ContextID,
                                                               cmsPSResourceType Type,
                                                               cmsHPROFILE hProfile,
                                                               cmsUInt32Number Intent,
                                                               cmsUInt32Number dwFlags,
                                                               cmsIOHANDLER* io)
{
    cmsUInt32Number  rc;


    switch (Type) {

        case cmsPS_RESOURCE_CSA:
            rc = GenerateCSA(ContextID, hProfile, Intent, dwFlags, io);
            break;

        default:
        case cmsPS_RESOURCE_CRD:
            rc = GenerateCRD(ContextID, hProfile, Intent, dwFlags, io);
            break;
    }

    return rc;
}