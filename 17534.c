void DL_Dxf::writeLine(DL_WriterA& dw,
                       const DL_LineData& data,
                       const DL_Attributes& attrib) {
    dw.entity("LINE");
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbLine");
    }
    dw.coord(DL_LINE_START_CODE, data.x1, data.y1, data.z1);
    dw.coord(DL_LINE_END_CODE, data.x2, data.y2, data.z2);
}