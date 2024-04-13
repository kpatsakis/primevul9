void DL_Dxf::writeBlockRecord(DL_WriterA& dw) {
    dw.dxfString(  0, "TABLE");
    dw.dxfString(  2, "BLOCK_RECORD");
    if (version==DL_VERSION_2000) {
        dw.dxfHex(5, 1);
    }
    //dw.dxfHex(330, 0);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbSymbolTable");
    }
    dw.dxfInt( 70, 1);

    dw.dxfString(  0, "BLOCK_RECORD");
    if (version==DL_VERSION_2000) {
        dw.dxfHex(5, 0x1F);
    }
    //int msh = dw.handle();
    //dw.setModelSpaceHandle(msh);
    //dw.dxfHex(330, 1);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbSymbolTableRecord");
        dw.dxfString(100, "AcDbBlockTableRecord");
    }
    dw.dxfString(  2, "*Model_Space");
    dw.dxfHex(340, 0x22);

    dw.dxfString(  0, "BLOCK_RECORD");
    if (version==DL_VERSION_2000) {
        dw.dxfHex(5, 0x1B);
    }
    //int psh = dw.handle();
    //dw.setPaperSpaceHandle(psh);
    //dw.dxfHex(330, 1);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbSymbolTableRecord");
        dw.dxfString(100, "AcDbBlockTableRecord");
    }
    dw.dxfString(  2, "*Paper_Space");
    dw.dxfHex(340, 0x1E);

    dw.dxfString(  0, "BLOCK_RECORD");
    if (version==DL_VERSION_2000) {
        dw.dxfHex(5, 0x23);
    }
    //int ps0h = dw.handle();
    //dw.setPaperSpace0Handle(ps0h);
    //dw.dxfHex(330, 1);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbSymbolTableRecord");
        dw.dxfString(100, "AcDbBlockTableRecord");
    }
    dw.dxfString(  2, "*Paper_Space0");
    dw.dxfHex(340, 0x26);

    //dw.dxfString(  0, "ENDTAB");
}