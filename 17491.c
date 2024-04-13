void DL_Dxf::addImage(DL_CreationInterface* creationInterface) {
    DL_ImageData id(// pass ref insead of name we don't have yet
        getStringValue(340, ""),
        // ins point:
        getRealValue(10, 0.0),
        getRealValue(20, 0.0),
        getRealValue(30, 0.0),
        // u vector:
        getRealValue(11, 1.0),
        getRealValue(21, 0.0),
        getRealValue(31, 0.0),
        // v vector:
        getRealValue(12, 0.0),
        getRealValue(22, 1.0),
        getRealValue(32, 0.0),
        // image size (pixel):
        getIntValue(13, 1),
        getIntValue(23, 1),
        // brightness, contrast, fade
        getIntValue(281, 50),
        getIntValue(282, 50),
        getIntValue(283, 0));

    creationInterface->addImage(id);
    creationInterface->endEntity();
    currentObjectType = DL_UNKNOWN;
}