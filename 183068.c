int EmitCIEBasedABC(cmsIOHANDLER* m, cmsFloat64Number* Matrix, cmsToneCurve** CurveSet, cmsCIEXYZ* BlackPoint)
{
    int i;

    _cmsIOPrintf(m, "[ /CIEBasedABC\n");
    _cmsIOPrintf(m, "<<\n");
    _cmsIOPrintf(m, "/DecodeABC [ ");

    EmitNGamma(m, 3, CurveSet);

    _cmsIOPrintf(m, "]\n");

    _cmsIOPrintf(m, "/MatrixABC [ " );

    for( i=0; i < 3; i++ ) {

        _cmsIOPrintf(m, "%.6f %.6f %.6f ", Matrix[i + 3*0],
                                           Matrix[i + 3*1],
                                           Matrix[i + 3*2]);
    }


    _cmsIOPrintf(m, "]\n");

    _cmsIOPrintf(m, "/RangeLMN [ 0.0 0.9642 0.0 1.0000 0.0 0.8249 ]\n");

    EmitWhiteBlackD50(m, BlackPoint);
    EmitIntent(m, INTENT_PERCEPTUAL);

    _cmsIOPrintf(m, ">>\n");
    _cmsIOPrintf(m, "]\n");


    return 1;
}