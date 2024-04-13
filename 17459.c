void DL_Dxf::writeBlock(DL_WriterA& dw, const DL_BlockData& data) {
    if (data.name.empty()) {
        std::cerr << "DL_Dxf::writeBlock: "
        << "Block name must not be empty\n";
        return;
    }

    std::string n = data.name;
    std::transform(n.begin(), n.end(), n.begin(), ::toupper);

    if (n=="*PAPER_SPACE") {
        dw.sectionBlockEntry(0x1C);
    } else if (n=="*MODEL_SPACE") {
        dw.sectionBlockEntry(0x20);
    } else if (n=="*PAPER_SPACE0") {
        dw.sectionBlockEntry(0x24);
    } else {
        dw.sectionBlockEntry();
    }
    dw.dxfString(2, data.name);
    dw.dxfInt(70, 0);
    dw.coord(10, data.bpx, data.bpy, data.bpz);
    dw.dxfString(3, data.name);
    dw.dxfString(1, "");
}