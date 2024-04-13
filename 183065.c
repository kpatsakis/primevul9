int EmitCIEBasedA(cmsIOHANDLER* m, cmsToneCurve* Curve, cmsCIEXYZ* BlackPoint)
{

    _cmsIOPrintf(m, "[ /CIEBasedA\n");
    _cmsIOPrintf(m, "  <<\n");

    _cmsIOPrintf(m, "/DecodeA ");

    Emit1Gamma(m, Curve);

    _cmsIOPrintf(m, " \n");

    _cmsIOPrintf(m, "/MatrixA [ 0.9642 1.0000 0.8249 ]\n");
    _cmsIOPrintf(m, "/RangeLMN [ 0.0 0.9642 0.0 1.0000 0.0 0.8249 ]\n");

    EmitWhiteBlackD50(m, BlackPoint);
    EmitIntent(m, INTENT_PERCEPTUAL);

    _cmsIOPrintf(m, ">>\n");
    _cmsIOPrintf(m, "]\n");

    return 1;
}