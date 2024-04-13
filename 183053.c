void EmitLab2XYZ(cmsIOHANDLER* m)
{
    _cmsIOPrintf(m, "/RangeABC [ 0 1 0 1 0 1]\n");
    _cmsIOPrintf(m, "/DecodeABC [\n");
    _cmsIOPrintf(m, "{100 mul  16 add 116 div } bind\n");
    _cmsIOPrintf(m, "{255 mul 128 sub 500 div } bind\n");
    _cmsIOPrintf(m, "{255 mul 128 sub 200 div } bind\n");
    _cmsIOPrintf(m, "]\n");
    _cmsIOPrintf(m, "/MatrixABC [ 1 1 1 1 0 0 0 0 -1]\n");
    _cmsIOPrintf(m, "/RangeLMN [ -0.236 1.254 0 1 -0.635 1.640 ]\n");
    _cmsIOPrintf(m, "/DecodeLMN [\n");
    _cmsIOPrintf(m, "{dup 6 29 div ge {dup dup mul mul} {4 29 div sub 108 841 div mul} ifelse 0.964200 mul} bind\n");
    _cmsIOPrintf(m, "{dup 6 29 div ge {dup dup mul mul} {4 29 div sub 108 841 div mul} ifelse } bind\n");
    _cmsIOPrintf(m, "{dup 6 29 div ge {dup dup mul mul} {4 29 div sub 108 841 div mul} ifelse 0.824900 mul} bind\n");
    _cmsIOPrintf(m, "]\n");
}