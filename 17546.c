void DL_Dxf::writeMText(DL_WriterA& dw,
                        const DL_MTextData& data,
                        const DL_Attributes& attrib) {

    dw.entity("MTEXT");
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbMText");
    }
    dw.dxfReal(10, data.ipx);
    dw.dxfReal(20, data.ipy);
    dw.dxfReal(30, data.ipz);
    dw.dxfReal(40, data.height);
    dw.dxfReal(41, data.width);

    dw.dxfInt(71, data.attachmentPoint);
    dw.dxfInt(72, data.drawingDirection);

    // Creare text chunks of 250 characters each:
    int length = data.text.length();
    char chunk[251];
    int i;
    for (i=250; i<length; i+=250) {
        strncpy(chunk, &data.text.c_str()[i-250], 250);
        chunk[250]='\0';
        dw.dxfString(3, chunk);
    }
    strncpy(chunk, &data.text.c_str()[i-250], 250);
    chunk[250]='\0';
    dw.dxfString(1, chunk);

    dw.dxfString(7, data.style);

    // since dxflib 2.0.2.1: degrees not rad (error in autodesk dxf doc)
    dw.dxfReal(50, data.angle/(2.0*M_PI)*360.0);

    dw.dxfInt(73, data.lineSpacingStyle);
    dw.dxfReal(44, data.lineSpacingFactor);
}