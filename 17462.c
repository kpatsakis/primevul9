void DL_Dxf::writeXRecord(DL_WriterA& dw, int handle, bool value) {
    dw.dxfString(  0, "XRECORD");
    dw.dxfHex(5, handle);
    dw.dxfHex(330, appDictionaryHandle);
    dw.dxfString(100, "AcDbXrecord");
    dw.dxfInt(280, 1);
    dw.dxfBool(290, value);
}