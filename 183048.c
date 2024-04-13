cmsUInt32Number CMSEXPORT cmsGetPostScriptCRD(cmsContext ContextID,
                              cmsHPROFILE hProfile,
                              cmsUInt32Number Intent, cmsUInt32Number dwFlags,
                              void* Buffer, cmsUInt32Number dwBufferLen)
{
    cmsIOHANDLER* mem;
    cmsUInt32Number dwBytesUsed;

    // Set up the serialization engine
    if (Buffer == NULL)
        mem = cmsOpenIOhandlerFromNULL(ContextID);
    else
        mem = cmsOpenIOhandlerFromMem(ContextID, Buffer, dwBufferLen, "w");

    if (!mem) return 0;

    dwBytesUsed =  cmsGetPostScriptColorResource(ContextID, cmsPS_RESOURCE_CRD, hProfile, Intent, dwFlags, mem);

    // Get rid of memory stream
    cmsCloseIOhandler(mem);

    return dwBytesUsed;
}