void DL_Dxf::writePolylineEnd(DL_WriterA& dw) {
    if (version==DL_VERSION_2000) {
    } else {
        dw.entity("SEQEND");
    }
}