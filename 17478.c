unsigned long DL_Dxf::writeDictionaryEntry(DL_WriterA& dw, const std::string& name) {
    dw.dxfString(  3, name);
    return dw.handle(350);
}