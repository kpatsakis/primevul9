void DL_Dxf::writeEndBlock(DL_WriterA& dw, const std::string& name) {
    std::string n = name;
    std::transform(n.begin(), n.end(), n.begin(), ::toupper);

    if (n=="*PAPER_SPACE") {
        dw.sectionBlockEntryEnd(0x1D);
    } else if (n=="*MODEL_SPACE") {
        dw.sectionBlockEntryEnd(0x21);
    } else if (n=="*PAPER_SPACE0") {
        dw.sectionBlockEntryEnd(0x25);
    } else {
        dw.sectionBlockEntryEnd();
    }
}