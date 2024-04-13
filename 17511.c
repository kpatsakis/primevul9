void DL_Dxf::writeAttribute(DL_WriterA& dw,
                   const DL_AttributeData& data,
                   const DL_Attributes& attrib) {

    dw.entity("ATTRIB");
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbText");
    }
    dw.dxfReal(10, data.ipx);
    dw.dxfReal(20, data.ipy);
    dw.dxfReal(30, data.ipz);
    dw.dxfReal(40, data.height);
    dw.dxfString(1, data.text);
    dw.dxfReal(50, data.angle/(2*M_PI)*360.0);
    dw.dxfReal(41, data.xScaleFactor);
    dw.dxfString(7, data.style);

    dw.dxfInt(71, data.textGenerationFlags);
    dw.dxfInt(72, data.hJustification);

    dw.dxfReal(11, data.apx);
    dw.dxfReal(21, data.apy);
    dw.dxfReal(31, data.apz);

    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbAttribute");
    }

    dw.dxfString(2, data.tag);
    dw.dxfInt(74, data.vJustification);
}