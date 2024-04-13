void DL_Dxf::writeCircle(DL_WriterA& dw,
                         const DL_CircleData& data,
                         const DL_Attributes& attrib) {
    dw.entity("CIRCLE");
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbCircle");
    }
    dw.coord(10, data.cx, data.cy, data.cz);
    dw.dxfReal(40, data.radius);
}