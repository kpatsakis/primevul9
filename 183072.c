void WriteCLUT(cmsIOHANDLER* m, cmsStage* mpe, const char* PreMaj,
                                             const char* PostMaj,
                                             const char* PreMin,
                                             const char* PostMin,
                                             int FixWhite,
                                             cmsColorSpaceSignature ColorSpace)
{
    cmsUInt32Number i;
    cmsPsSamplerCargo sc;

    sc.FirstComponent = -1;
    sc.SecondComponent = -1;
    sc.Pipeline = (_cmsStageCLutData *) mpe ->Data;
    sc.m   = m;
    sc.PreMaj = PreMaj;
    sc.PostMaj= PostMaj;

    sc.PreMin   = PreMin;
    sc.PostMin  = PostMin;
    sc.FixWhite = FixWhite;
    sc.ColorSpace = ColorSpace;

    _cmsIOPrintf(m, "[");

    for (i=0; i < sc.Pipeline->Params->nInputs; i++)
        _cmsIOPrintf(m, " %d ", sc.Pipeline->Params->nSamples[i]);

    _cmsIOPrintf(m, " [\n");

    cmsStageSampleCLut16bit(mpe, OutputValueSampler, (void*) &sc, SAMPLER_INSPECT);

    _cmsIOPrintf(m, PostMin);
    _cmsIOPrintf(m, PostMaj);
    _cmsIOPrintf(m, "] ");

}