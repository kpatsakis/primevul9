void DL_Dxf::writeEllipse(DL_WriterA& dw,
                          const DL_EllipseData& data,
                          const DL_Attributes& attrib) {

    if (version>DL_VERSION_R12) {
        dw.entity("ELLIPSE");
        if (version==DL_VERSION_2000) {
            dw.dxfString(100, "AcDbEntity");
        }
        dw.entityAttributes(attrib);
        if (version==DL_VERSION_2000) {
            dw.dxfString(100, "AcDbEllipse");
        }
        dw.coord(10, data.cx, data.cy, data.cz);
        dw.coord(11, data.mx, data.my, data.mz);
        dw.dxfReal(40, data.ratio);
        dw.dxfReal(41, data.angle1);
        dw.dxfReal(42, data.angle2);
    }
}