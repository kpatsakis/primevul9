void DL_Dxf::writeDimDiametric(DL_WriterA& dw,
                               const DL_DimensionData& data,
                               const DL_DimDiametricData& edata,
                               const DL_Attributes& attrib) {

    dw.entity("DIMENSION");

    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbDimension");
    }

    dw.dxfReal(10, data.dpx);
    dw.dxfReal(20, data.dpy);
    dw.dxfReal(30, data.dpz);

    dw.dxfReal(11, data.mpx);
    dw.dxfReal(21, data.mpy);
    dw.dxfReal(31, 0.0);

    dw.dxfInt(70, data.type);
    if (version>DL_VERSION_R12) {
        dw.dxfInt(71, data.attachmentPoint);
        dw.dxfInt(72, data.lineSpacingStyle); // opt
        dw.dxfInt(74, data.arrow1Flipped);
        dw.dxfInt(75, data.arrow2Flipped);
        dw.dxfReal(41, data.lineSpacingFactor); // opt
    }

    dw.dxfReal(42, data.angle);

    dw.dxfString(1, data.text);   // opt
    //dw.dxfString(3, data.style);
    dw.dxfString(3, "Standard");

    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbDiametricDimension");
    }

    dw.dxfReal(15, edata.dpx);
    dw.dxfReal(25, edata.dpy);
    dw.dxfReal(35, 0.0);

    dw.dxfReal(40, edata.leader);

    writeDimStyleOverrides(dw, data);
}