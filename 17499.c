void DL_Dxf::writeArc(DL_WriterA& dw,
                      const DL_ArcData& data,
                      const DL_Attributes& attrib) {
    dw.entity("ARC");
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbCircle");
    }
    dw.coord(10, data.cx, data.cy, data.cz);
    dw.dxfReal(40, data.radius);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbArc");
    }
    dw.dxfReal(50, data.angle1);
    dw.dxfReal(51, data.angle2);
}