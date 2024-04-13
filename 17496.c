void DL_Dxf::writeAppid(DL_WriterA& dw, const std::string& name) {
    if (name.empty()) {
        std::cerr << "DL_Dxf::writeAppid: "
        << "Application  name must not be empty\n";
        return;
    }

    std::string n = name;
    std::transform(n.begin(), n.end(), n.begin(), ::toupper);

    if (n=="ACAD") {
        dw.tableAppidEntry(0x12);
    } else {
        dw.tableAppidEntry();
    }
    dw.dxfString(2, name);
    dw.dxfInt(70, 0);
}