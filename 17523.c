void DL_Dxf::writeVPort(DL_WriterA& dw) {
    dw.dxfString(0, "TABLE");
    dw.dxfString(2, "VPORT");
    if (version==DL_VERSION_2000) {
        dw.dxfHex(5, 0x8);
    }
    //dw.dxfHex(330, 0);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbSymbolTable");
    }
    dw.dxfInt(70, 1);
    dw.dxfString(0, "VPORT");
    //dw.dxfHex(5, 0x2F);
    if (version==DL_VERSION_2000) {
        dw.handle();
    }
    //dw.dxfHex(330, 8);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbSymbolTableRecord");
        dw.dxfString(100, "AcDbViewportTableRecord");
    }
    dw.dxfString(  2, "*Active");
    dw.dxfInt( 70, 0);
    dw.dxfReal( 10, 0.0);
    dw.dxfReal( 20, 0.0);
    dw.dxfReal( 11, 1.0);
    dw.dxfReal( 21, 1.0);
    dw.dxfReal( 12, 286.3055555555555);
    dw.dxfReal( 22, 148.5);
    dw.dxfReal( 13, 0.0);
    dw.dxfReal( 23, 0.0);
    dw.dxfReal( 14, 10.0);
    dw.dxfReal( 24, 10.0);
    dw.dxfReal( 15, 10.0);
    dw.dxfReal( 25, 10.0);
    dw.dxfReal( 16, 0.0);
    dw.dxfReal( 26, 0.0);
    dw.dxfReal( 36, 1.0);
    dw.dxfReal( 17, 0.0);
    dw.dxfReal( 27, 0.0);
    dw.dxfReal( 37, 0.0);
    dw.dxfReal( 40, 297.0);
    dw.dxfReal( 41, 1.92798353909465);
    dw.dxfReal( 42, 50.0);
    dw.dxfReal( 43, 0.0);
    dw.dxfReal( 44, 0.0);
    dw.dxfReal( 50, 0.0);
    dw.dxfReal( 51, 0.0);
    dw.dxfInt( 71, 0);
    dw.dxfInt( 72, 100);
    dw.dxfInt( 73, 1);
    dw.dxfInt( 74, 3);
    dw.dxfInt( 75, 1);
    dw.dxfInt( 76, 1);
    dw.dxfInt( 77, 0);
    dw.dxfInt( 78, 0);

    if (version==DL_VERSION_2000) {
        dw.dxfInt(281, 0);
        dw.dxfInt( 65, 1);
        dw.dxfReal(110, 0.0);
        dw.dxfReal(120, 0.0);
        dw.dxfReal(130, 0.0);
        dw.dxfReal(111, 1.0);
        dw.dxfReal(121, 0.0);
        dw.dxfReal(131, 0.0);
        dw.dxfReal(112, 0.0);
        dw.dxfReal(122, 1.0);
        dw.dxfReal(132, 0.0);
        dw.dxfInt( 79, 0);
        dw.dxfReal(146, 0.0);
    }
    dw.dxfString(  0, "ENDTAB");
}