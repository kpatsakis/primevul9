void DL_Dxf::writeHatch1(DL_WriterA& dw,
                         const DL_HatchData& data,
                         const DL_Attributes& attrib) {

    dw.entity("HATCH");
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbHatch");
    }
    dw.dxfReal(10, 0.0);             // elevation
    dw.dxfReal(20, 0.0);
    dw.dxfReal(30, 0.0);
    dw.dxfReal(210, 0.0);             // extrusion dir.
    dw.dxfReal(220, 0.0);
    dw.dxfReal(230, 1.0);
    if (data.solid==false) {
        dw.dxfString(2, data.pattern);
    } else {
        dw.dxfString(2, "SOLID");
    }
    dw.dxfInt(70, (int)data.solid);
    dw.dxfInt(71, 0);                // non-associative
    dw.dxfInt(91, data.numLoops);
}