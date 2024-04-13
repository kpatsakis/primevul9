void DL_Dxf::writeAppDictionary(DL_WriterA& dw) {
    dw.dxfString(  0, "DICTIONARY");
    //dw.handle();
    dw.dxfHex(5, appDictionaryHandle);
    dw.dxfString(100, "AcDbDictionary");
    dw.dxfInt(281, 1);
}