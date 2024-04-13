void DL_Dxf::writeXLine(DL_WriterA& dw,
                       const DL_XLineData& data,
                       const DL_Attributes& attrib) {
    dw.entity("XLINE");
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbLine");
    }
    dw.coord(DL_LINE_START_CODE, data.bx, data.by, data.bz);
    dw.coord(DL_LINE_END_CODE, data.dx, data.dy, data.dz);
}