void DL_Dxf::writeDimStyle(DL_WriterA& dw,
                    double dimasz, double dimexe, double dimexo,
                    double dimgap, double dimtxt) {

    dw.dxfString(  0, "TABLE");
    dw.dxfString(  2, "DIMSTYLE");
    if (version==DL_VERSION_2000) {
        dw.dxfHex(5, 0xA);
        dw.dxfString(100, "AcDbSymbolTable");
    }
    dw.dxfInt( 70, 1);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbDimStyleTable");
        dw.dxfInt( 71, 0);
    }


    dw.dxfString(  0, "DIMSTYLE");
    if (version==DL_VERSION_2000) {
        dw.dxfHex(105, 0x27);
    }
    //dw.handle(105);
    //dw.dxfHex(330, 0xA);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbSymbolTableRecord");
        dw.dxfString(100, "AcDbDimStyleTableRecord");
    }
    dw.dxfString(  2, "Standard");
    if (version==DL_VERSION_R12) {
        dw.dxfString(  3, "");
        dw.dxfString(  4, "");
        dw.dxfString(  5, "");
        dw.dxfString(  6, "");
        dw.dxfString(  7, "");
        dw.dxfReal( 40, 1.0);
    }

    dw.dxfReal( 41, dimasz);
    dw.dxfReal( 42, dimexo);
    dw.dxfReal( 43, 3.75);
    dw.dxfReal( 44, dimexe);
    if (version==DL_VERSION_R12) {
        dw.dxfReal( 45, 0.0);
        dw.dxfReal( 46, 0.0);
        dw.dxfReal( 47, 0.0);
        dw.dxfReal( 48, 0.0);
    }
    dw.dxfInt( 70, 0);
    if (version==DL_VERSION_R12) {
        dw.dxfInt( 71, 0);
        dw.dxfInt( 72, 0);
    }
    dw.dxfInt( 73, 0);
    dw.dxfInt( 74, 0);
    if (version==DL_VERSION_R12) {
        dw.dxfInt( 75, 0);
        dw.dxfInt( 76, 0);
    }
    dw.dxfInt( 77, 1);
    dw.dxfInt( 78, 8);
    dw.dxfReal(140, dimtxt);
    dw.dxfReal(141, 2.5);
    if (version==DL_VERSION_R12) {
        dw.dxfReal(142, 0.0);
    }
    dw.dxfReal(143, 0.03937007874016);
    if (version==DL_VERSION_R12) {
        dw.dxfReal(144, 1.0);
        dw.dxfReal(145, 0.0);
        dw.dxfReal(146, 1.0);
    }
    dw.dxfReal(147, dimgap);
    if (version==DL_VERSION_R12) {
        dw.dxfInt(170, 0);
    }
    dw.dxfInt(171, 3);
    dw.dxfInt(172, 1);
    if (version==DL_VERSION_R12) {
        dw.dxfInt(173, 0);
        dw.dxfInt(174, 0);
        dw.dxfInt(175, 0);
        dw.dxfInt(176, 0);
        dw.dxfInt(177, 0);
        dw.dxfInt(178, 0);
    }
    if (version==DL_VERSION_2000) {
        dw.dxfInt(271, 2);
        dw.dxfInt(272, 2);
        dw.dxfInt(274, 3);
        dw.dxfInt(278, 44);
        dw.dxfInt(283, 0);
        dw.dxfInt(284, 8);
        dw.dxfHex(340, styleHandleStd);
        //dw.dxfHex(340, 0x11);
    }
    // * /
    dw.dxfString(  0, "ENDTAB");
}