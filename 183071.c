int EmitCIEBasedDEF(cmsIOHANDLER* m, cmsPipeline* Pipeline, cmsUInt32Number Intent, cmsCIEXYZ* BlackPoint)
{
    const char* PreMaj;
    const char* PostMaj;
    const char* PreMin, *PostMin;
    cmsStage* mpe;

    mpe = Pipeline ->Elements;

    switch (cmsStageInputChannels(mpe)) {
    case 3:

            _cmsIOPrintf(m, "[ /CIEBasedDEF\n");
            PreMaj ="<";
            PostMaj= ">\n";
            PreMin = PostMin = "";
            break;
    case 4:
            _cmsIOPrintf(m, "[ /CIEBasedDEFG\n");
            PreMaj = "[";
            PostMaj = "]\n";
            PreMin = "<";
            PostMin = ">\n";
            break;
    default:
            return 0;

    }

    _cmsIOPrintf(m, "<<\n");

    if (cmsStageType(mpe) == cmsSigCurveSetElemType) {

        _cmsIOPrintf(m, "/DecodeDEF [ ");
        EmitNGamma(m, cmsStageOutputChannels(mpe), _cmsStageGetPtrToCurveSet(mpe));
        _cmsIOPrintf(m, "]\n");

        mpe = mpe ->Next;
    }

    if (cmsStageType(mpe) == cmsSigCLutElemType) {

            _cmsIOPrintf(m, "/Table ");
            WriteCLUT(m, mpe, PreMaj, PostMaj, PreMin, PostMin, FALSE, (cmsColorSpaceSignature) 0);
            _cmsIOPrintf(m, "]\n");
    }

    EmitLab2XYZ(m);
    EmitWhiteBlackD50(m, BlackPoint);
    EmitIntent(m, Intent);

    _cmsIOPrintf(m, "   >>\n");
    _cmsIOPrintf(m, "]\n");

    return 1;
}