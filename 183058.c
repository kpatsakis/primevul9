void EmitPQRStage(cmsIOHANDLER* m, cmsHPROFILE hProfile, int DoBPC, int lIsAbsolute)
{


        if (lIsAbsolute) {

            // For absolute colorimetric intent, encode back to relative
            // and generate a relative Pipeline

            // Relative encoding is obtained across XYZpcs*(D50/WhitePoint)

            cmsCIEXYZ White;

            _cmsReadMediaWhitePoint(&White, hProfile);

            _cmsIOPrintf(m,"/MatrixPQR [1 0 0 0 1 0 0 0 1 ]\n");
            _cmsIOPrintf(m,"/RangePQR [ -0.5 2 -0.5 2 -0.5 2 ]\n");

            _cmsIOPrintf(m, "%% Absolute colorimetric -- encode to relative to maximize LUT usage\n"
                      "/TransformPQR [\n"
                      "{0.9642 mul %g div exch pop exch pop exch pop exch pop} bind\n"
                      "{1.0000 mul %g div exch pop exch pop exch pop exch pop} bind\n"
                      "{0.8249 mul %g div exch pop exch pop exch pop exch pop} bind\n]\n",
                      White.X, White.Y, White.Z);
            return;
        }


        _cmsIOPrintf(m,"%% Bradford Cone Space\n"
                 "/MatrixPQR [0.8951 -0.7502 0.0389 0.2664 1.7135 -0.0685 -0.1614 0.0367 1.0296 ] \n");

        _cmsIOPrintf(m, "/RangePQR [ -0.5 2 -0.5 2 -0.5 2 ]\n");


        // No BPC

        if (!DoBPC) {

            _cmsIOPrintf(m, "%% VonKries-like transform in Bradford Cone Space\n"
                      "/TransformPQR [\n"
                      "{exch pop exch 3 get mul exch pop exch 3 get div} bind\n"
                      "{exch pop exch 4 get mul exch pop exch 4 get div} bind\n"
                      "{exch pop exch 5 get mul exch pop exch 5 get div} bind\n]\n");
        } else {

            // BPC

            _cmsIOPrintf(m, "%% VonKries-like transform in Bradford Cone Space plus BPC\n"
                      "/TransformPQR [\n");

            _cmsIOPrintf(m, "{4 index 3 get div 2 index 3 get mul "
                    "2 index 3 get 2 index 3 get sub mul "
                    "2 index 3 get 4 index 3 get 3 index 3 get sub mul sub "
                    "3 index 3 get 3 index 3 get exch sub div "
                    "exch pop exch pop exch pop exch pop } bind\n");

            _cmsIOPrintf(m, "{4 index 4 get div 2 index 4 get mul "
                    "2 index 4 get 2 index 4 get sub mul "
                    "2 index 4 get 4 index 4 get 3 index 4 get sub mul sub "
                    "3 index 4 get 3 index 4 get exch sub div "
                    "exch pop exch pop exch pop exch pop } bind\n");

            _cmsIOPrintf(m, "{4 index 5 get div 2 index 5 get mul "
                    "2 index 5 get 2 index 5 get sub mul "
                    "2 index 5 get 4 index 5 get 3 index 5 get sub mul sub "
                    "3 index 5 get 3 index 5 get exch sub div "
                    "exch pop exch pop exch pop exch pop } bind\n]\n");

        }


}