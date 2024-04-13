void DL_Dxf::writeControlPoint(DL_WriterA& dw,
                               const DL_ControlPointData& data) {

    dw.dxfReal(10, data.x);
    dw.dxfReal(20, data.y);
    dw.dxfReal(30, data.z);
}