void DL_Dxf::writeComment(DL_WriterA& dw, const std::string& comment) {
    dw.dxfString(999, comment);
}