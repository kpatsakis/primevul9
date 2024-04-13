void EmitWhiteBlackD50(cmsIOHANDLER* m, cmsCIEXYZ* BlackPoint)
{

    _cmsIOPrintf(m, "/BlackPoint [%f %f %f]\n", BlackPoint -> X,
                                          BlackPoint -> Y,
                                          BlackPoint -> Z);

    _cmsIOPrintf(m, "/WhitePoint [%f %f %f]\n", cmsD50_XYZ()->X,
                                          cmsD50_XYZ()->Y,
                                          cmsD50_XYZ()->Z);
}