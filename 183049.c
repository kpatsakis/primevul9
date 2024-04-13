void EmitHeader(cmsIOHANDLER* m, const char* Title, cmsHPROFILE hProfile)
{
    time_t timer;
    cmsMLU *Description, *Copyright;
    char DescASCII[256], CopyrightASCII[256];

    time(&timer);

    Description = (cmsMLU*) cmsReadTag(hProfile, cmsSigProfileDescriptionTag);
    Copyright   = (cmsMLU*) cmsReadTag(hProfile, cmsSigCopyrightTag);

    DescASCII[0] = DescASCII[255] = 0;
    CopyrightASCII[0] = CopyrightASCII[255] = 0;

    if (Description != NULL) cmsMLUgetASCII(Description,  cmsNoLanguage, cmsNoCountry, DescASCII,       255);
    if (Copyright != NULL)   cmsMLUgetASCII(Copyright,    cmsNoLanguage, cmsNoCountry, CopyrightASCII,  255);

    _cmsIOPrintf(m, "%%!PS-Adobe-3.0\n");
    _cmsIOPrintf(m, "%%\n");
    _cmsIOPrintf(m, "%% %s\n", Title);
    _cmsIOPrintf(m, "%% Source: %s\n", RemoveCR(DescASCII));
    _cmsIOPrintf(m, "%%         %s\n", RemoveCR(CopyrightASCII));
    _cmsIOPrintf(m, "%% Created: %s", ctime(&timer)); // ctime appends a \n!!!
    _cmsIOPrintf(m, "%%\n");
    _cmsIOPrintf(m, "%%%%BeginResource\n");

}