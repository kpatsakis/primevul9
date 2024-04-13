int WriteNamedColorCSA(cmsIOHANDLER* m, cmsHPROFILE hNamedColor, cmsUInt32Number Intent)
{
    cmsHTRANSFORM xform;
    cmsHPROFILE   hLab;
    cmsUInt32Number i, nColors;
    char ColorName[cmsMAX_PATH];
    cmsNAMEDCOLORLIST* NamedColorList;

    hLab  = cmsCreateLab4ProfileTHR(m ->ContextID, NULL);
    xform = cmsCreateTransform(hNamedColor, TYPE_NAMED_COLOR_INDEX, hLab, TYPE_Lab_DBL, Intent, 0);
    if (xform == NULL) return 0;

    NamedColorList = cmsGetNamedColorList(xform);
    if (NamedColorList == NULL) return 0;

    _cmsIOPrintf(m, "<<\n");
    _cmsIOPrintf(m, "(colorlistcomment) (%s)\n", "Named color CSA");
    _cmsIOPrintf(m, "(Prefix) [ (Pantone ) (PANTONE ) ]\n");
    _cmsIOPrintf(m, "(Suffix) [ ( CV) ( CVC) ( C) ]\n");

    nColors   = cmsNamedColorCount(NamedColorList);


    for (i=0; i < nColors; i++) {

        cmsUInt16Number In[1];
        cmsCIELab Lab;

        In[0] = (cmsUInt16Number) i;

        if (!cmsNamedColorInfo(NamedColorList, i, ColorName, NULL, NULL, NULL, NULL))
                continue;

        cmsDoTransform(xform, In, &Lab, 1);
        _cmsIOPrintf(m, "  (%s) [ %.3f %.3f %.3f ]\n", ColorName, Lab.L, Lab.a, Lab.b);
    }



    _cmsIOPrintf(m, ">>\n");

    cmsDeleteTransform(xform);
    cmsCloseProfile(hLab);
    return 1;
}