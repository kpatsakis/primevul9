void DL_Dxf::writeDimLinear(DL_WriterA& dw,
                            const DL_DimensionData& data,
                            const DL_DimLinearData& edata,
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
        dw.dxfString(100, "AcDbAlignedDimension");
    }

    dw.dxfReal(13, edata.dpx1);
    dw.dxfReal(23, edata.dpy1);
    dw.dxfReal(33, 0.0);

    dw.dxfReal(14, edata.dpx2);
    dw.dxfReal(24, edata.dpy2);
    dw.dxfReal(34, 0.0);

    dw.dxfReal(50, edata.angle/(2.0*M_PI)*360.0);

    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbRotatedDimension");
    }

    writeDimStyleOverrides(dw, data);
}