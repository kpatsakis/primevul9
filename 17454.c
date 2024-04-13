void DL_Dxf::writeHeader(DL_WriterA& dw) {
    dw.comment("dxflib " DL_VERSION);
    dw.sectionHeader();

    dw.dxfString(9, "$ACADVER");
    switch (version) {
    case DL_Codes::AC1009:
        dw.dxfString(1, "AC1009");
        break;
    case DL_Codes::AC1012:
        dw.dxfString(1, "AC1012");
        break;
    case DL_Codes::AC1014:
        dw.dxfString(1, "AC1014");
        break;
    case DL_Codes::AC1015:
        dw.dxfString(1, "AC1015");
        break;
    case DL_Codes::AC1009_MIN:
        // minimalistic DXF version is unidentified in file:
        break;
    }

    // Newer version require that (otherwise a*cad crashes..)
    if (version==DL_VERSION_2000) {
        dw.dxfString(9, "$HANDSEED");
        dw.dxfHex(5, 0xFFFF);
    }

    // commented out: more variables can be added after that by caller:
    //dw.sectionEnd();
}