void DL_Dxf::writeImageDef(DL_WriterA& dw,
                           int handle,
                           const DL_ImageData& data) {

    /*if (data.file.empty()) {
        std::cerr << "DL_Dxf::writeImage: "
        << "Image file must not be empty\n";
        return;
}*/

    dw.dxfString(0, "IMAGEDEF");
    if (version==DL_VERSION_2000) {
        dw.dxfHex(5, handle);
    }

    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbRasterImageDef");
        dw.dxfInt(90, 0);
    }
    // file name:
    dw.dxfString(1, data.ref);

    // image size in pixel
    dw.dxfReal(10, data.width);
    dw.dxfReal(20, data.height);

    dw.dxfReal(11, 1.0);
    dw.dxfReal(21, 1.0);

    // loaded:
    dw.dxfInt(280, 1);
    // units:
    dw.dxfInt(281, 0);
}