void DL_Dxf::writeXRecord(DL_WriterA& dw, int handle, const std::string& value) {
    dw.dxfString(  0, "XRECORD");
    dw.dxfHex(5, handle);
    dw.dxfHex(330, appDictionaryHandle);
    dw.dxfString(100, "AcDbXrecord");
    dw.dxfInt(280, 1);
    dw.dxfString(1000, value);
}