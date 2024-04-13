void DL_Dxf::addTextStyle(DL_CreationInterface* creationInterface) {
    std::string name = getStringValue(2, "");
    if (name.length()==0) {
        return;
    }

    DL_StyleData d(
        // name:
        name,
        // flags
        getIntValue(70, 0),
        // fixed text heigth:
        getRealValue(40, 0.0),
        // width factor:
        getRealValue(41, 0.0),
        // oblique angle:
        getRealValue(50, 0.0),
        // text generation flags:
        getIntValue(71, 0),
        // last height used:
        getRealValue(42, 0.0),
        // primart font file:
        getStringValue(3, ""),
        // big font file:
        getStringValue(4, "")
        );
    creationInterface->addTextStyle(d);
}