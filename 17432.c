void DL_Dxf::writeView(DL_WriterA& dw) {
    dw.dxfString(  0, "TABLE");
    dw.dxfString(  2, "VIEW");
    if (version==DL_VERSION_2000) {
        dw.dxfHex(5, 6);
    }
    //dw.dxfHex(330, 0);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbSymbolTable");
    }
    dw.dxfInt( 70, 0);
    dw.dxfString(  0, "ENDTAB");
}