void DL_Dxf::writeKnot(DL_WriterA& dw,
                       const DL_KnotData& data) {

    dw.dxfReal(40, data.k);
}