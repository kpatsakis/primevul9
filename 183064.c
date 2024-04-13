void EmitXYZ2Lab(cmsIOHANDLER* m)
{
    _cmsIOPrintf(m, "/RangeLMN [ -0.635 2.0 0 2 -0.635 2.0 ]\n");
    _cmsIOPrintf(m, "/EncodeLMN [\n");
    _cmsIOPrintf(m, "{ 0.964200  div dup 0.008856 le {7.787 mul 16 116 div add}{1 3 div exp} ifelse } bind\n");
    _cmsIOPrintf(m, "{ 1.000000  div dup 0.008856 le {7.787 mul 16 116 div add}{1 3 div exp} ifelse } bind\n");
    _cmsIOPrintf(m, "{ 0.824900  div dup 0.008856 le {7.787 mul 16 116 div add}{1 3 div exp} ifelse } bind\n");
    _cmsIOPrintf(m, "]\n");
    _cmsIOPrintf(m, "/MatrixABC [ 0 1 0 1 -1 1 0 0 -1 ]\n");
    _cmsIOPrintf(m, "/EncodeABC [\n");


    _cmsIOPrintf(m, "{ 116 mul  16 sub 100 div  } bind\n");
    _cmsIOPrintf(m, "{ 500 mul 128 add 256 div  } bind\n");
    _cmsIOPrintf(m, "{ 200 mul 128 add 256 div  } bind\n");


    _cmsIOPrintf(m, "]\n");


}