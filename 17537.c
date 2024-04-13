void DL_Dxf::writeHatchLoop1(DL_WriterA& dw,
                             const DL_HatchLoopData& data) {

    dw.dxfInt(92, 1);
    dw.dxfInt(93, data.numEdges);
    //dw.dxfInt(97, 0);
}