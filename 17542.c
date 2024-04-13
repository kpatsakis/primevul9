void DL_Dxf::writeHatch2(DL_WriterA& dw,
                         const DL_HatchData& data,
                         const DL_Attributes& /*attrib*/) {

    dw.dxfInt(75, 0);                // odd parity
    dw.dxfInt(76, 1);                // pattern type
    if (data.solid==false) {
        dw.dxfReal(52, data.angle);
        dw.dxfReal(41, data.scale);
        dw.dxfInt(77, 0);            // not double
        //dw.dxfInt(78, 0);
        dw.dxfInt(78, 1);
        dw.dxfReal(53, 45.0);
        dw.dxfReal(43, 0.0);
        dw.dxfReal(44, 0.0);
        dw.dxfReal(45, -0.0883883476483184);
        dw.dxfReal(46, 0.0883883476483185);
        dw.dxfInt(79, 0);
    }
    dw.dxfInt(98, 0);

    if (version==DL_VERSION_2000) {
        dw.dxfString(1001, "ACAD");
        dw.dxfReal(1010, data.originX);
        dw.dxfReal(1020, data.originY);
        dw.dxfInt(1030, 0.0);
    }
}