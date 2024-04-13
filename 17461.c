void DL_Dxf::writeLayer(DL_WriterA& dw,
                        const DL_LayerData& data,
                        const DL_Attributes& attrib) {

    if (data.name.empty()) {
        std::cerr << "DL_Dxf::writeLayer: "
        << "Layer name must not be empty\n";
        return;
    }

    int color = attrib.getColor();
    if (color>=256) {
        std::cerr << "Layer color cannot be " << color << ". Changed to 7.\n";
        color = 7;
    }
    if (data.off) {
        // negative color value means layer is off:
        color = -color;
    }

    if (data.name == "0") {
        dw.tableLayerEntry(0x10);
    } else {
        dw.tableLayerEntry();
    }

    dw.dxfString(2, data.name);
    dw.dxfInt(70, data.flags);
    dw.dxfInt(62, color);
    if (version>=DL_VERSION_2000 && attrib.getColor24()!=-1) {
        dw.dxfInt(420, attrib.getColor24());
    }

    dw.dxfString(6, (attrib.getLinetype().length()==0 ?
                     std::string("CONTINUOUS") : attrib.getLinetype()));

    if (version>=DL_VERSION_2000) {
        // layer defpoints cannot be plotted
        std::string lstr = data.name;
        std::transform(lstr.begin(), lstr.end(), lstr.begin(), tolower);
        if (lstr=="defpoints") {
            dw.dxfInt(290, 0);
        }
    }
    if (version>=DL_VERSION_2000 && attrib.getWidth()!=-1) {
        dw.dxfInt(370, attrib.getWidth());
    }
    if (version>=DL_VERSION_2000) {
        dw.dxfHex(390, 0xF);
    }
}