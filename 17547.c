void DL_Dxf::addBlock(DL_CreationInterface* creationInterface) {
    std::string name = getStringValue(2, "");
    if (name.length()==0) {
        return;
    }

    DL_BlockData d(
        // Name:
        name,
        // flags:
        getIntValue(70, 0),
        // base point:
        getRealValue(10, 0.0),
        getRealValue(20, 0.0),
        getRealValue(30, 0.0));

    creationInterface->addBlock(d);
}