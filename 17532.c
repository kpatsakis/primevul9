unsigned long DL_Dxf::writeImage(DL_WriterA& dw,
                       const DL_ImageData& data,
                       const DL_Attributes& attrib) {

    /*if (data.file.empty()) {
        std::cerr << "DL_Dxf::writeImage: "
        << "Image file must not be empty\n";
        return;
}*/

    dw.entity("IMAGE");

    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbRasterImage");
        dw.dxfInt(90, 0);
    }
    // insertion point
    dw.dxfReal(10, data.ipx);
    dw.dxfReal(20, data.ipy);
    dw.dxfReal(30, data.ipz);

    // vector along bottom side (1 pixel long)
    dw.dxfReal(11, data.ux);
    dw.dxfReal(21, data.uy);
    dw.dxfReal(31, data.uz);

    // vector along left side (1 pixel long)
    dw.dxfReal(12, data.vx);
    dw.dxfReal(22, data.vy);
    dw.dxfReal(32, data.vz);

    // image size in pixel
    dw.dxfReal(13, data.width);
    dw.dxfReal(23, data.height);

    // handle of IMAGEDEF object
    unsigned long handle = dw.handle(340);

    // flags
    dw.dxfInt(70, 15);

    // clipping:
    dw.dxfInt(280, 0);

    // brightness, contrast, fade
    dw.dxfInt(281, data.brightness);
    dw.dxfInt(282, data.contrast);
    dw.dxfInt(283, data.fade);

    return handle;
}