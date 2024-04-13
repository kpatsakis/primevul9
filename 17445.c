void DL_Dxf::writeLinetype(DL_WriterA& dw,
                           const DL_LinetypeData& data) {

    std::string nameUpper = data.name;
    std::transform(nameUpper.begin(), nameUpper.end(), nameUpper.begin(), ::toupper);

    if (data.name.empty()) {
        std::cerr << "DL_Dxf::writeLinetype: "
        << "Line type name must not be empty\n";
        return;
    }

    // ignore BYLAYER, BYBLOCK for R12
    if (version<DL_VERSION_2000) {
        if (nameUpper=="BYBLOCK" || nameUpper=="BYLAYER") {
            return;
        }
    }

    // write id (not for R12)
    if (nameUpper=="BYBLOCK") {
        dw.tableLinetypeEntry(0x14);
    } else if (nameUpper=="BYLAYER") {
        dw.tableLinetypeEntry(0x15);
    } else if (nameUpper=="CONTINUOUS") {
        dw.tableLinetypeEntry(0x16);
    } else {
        dw.tableLinetypeEntry();
    }

    dw.dxfString(2, data.name);
    dw.dxfInt(70, data.flags);

    if (nameUpper=="BYBLOCK") {
        dw.dxfString(3, "");
        dw.dxfInt(72, 65);
        dw.dxfInt(73, 0);
        dw.dxfReal(40, 0.0);
    } else if (nameUpper=="BYLAYER") {
        dw.dxfString(3, "");
        dw.dxfInt(72, 65);
        dw.dxfInt(73, 0);
        dw.dxfReal(40, 0.0);
    } else if (nameUpper=="CONTINUOUS") {
        dw.dxfString(3, "Solid line");
        dw.dxfInt(72, 65);
        dw.dxfInt(73, 0);
        dw.dxfReal(40, 0.0);
    } else {
        dw.dxfString(3, data.description);
        dw.dxfInt(72, 65);
        dw.dxfInt(73, data.numberOfDashes);
        dw.dxfReal(40, data.patternLength);
        for (int i = 0; i < data.numberOfDashes; i++) {
            dw.dxfReal(49, data.pattern[i]);
            if (version>=DL_VERSION_R13) {
                dw.dxfInt(74, 0);
            }
        }
    }
}