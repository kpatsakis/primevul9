void DL_Dxf::addAttribute(DL_CreationInterface* creationInterface) {
    DL_AttributeData d(
        // insertion point
        getRealValue(10, 0.0),
        getRealValue(20, 0.0),
        getRealValue(30, 0.0),
        // alignment point
        getRealValue(11, 0.0),
        getRealValue(21, 0.0),
        getRealValue(31, 0.0),
        // height
        getRealValue(40, 2.5),
        // x scale
        getRealValue(41, 1.0),
        // generation flags
        getIntValue(71, 0),
        // h just
        getIntValue(72, 0),
        // v just
        getIntValue(74, 0),
        // tag
        getStringValue(2, ""),
        // text
        getStringValue(1, ""),
        // style
        getStringValue(7, ""),
        // angle
        (getRealValue(50, 0.0)*2*M_PI)/360.0);

    creationInterface->addAttribute(d);
}