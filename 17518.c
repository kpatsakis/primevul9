void DL_Dxf::writeFitPoint(DL_WriterA& dw,
                           const DL_FitPointData& data) {

    dw.dxfReal(11, data.x);
    dw.dxfReal(21, data.y);
    dw.dxfReal(31, data.z);
}