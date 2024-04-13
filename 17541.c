void DL_Dxf::addText(DL_CreationInterface* creationInterface) {
    DL_TextData d(
        // insertion point
        getRealValue(10, 0.0),
        getRealValue(20, 0.0),
        getRealValue(30, 0.0),
        // alignment point
        getRealValue(11, DL_NANDOUBLE),
        getRealValue(21, DL_NANDOUBLE),
        getRealValue(31, DL_NANDOUBLE),
        // height
        getRealValue(40, 2.5),
        // x scale
        getRealValue(41, 1.0),
        // generation flags
        getIntValue(71, 0),
        // h just
        getIntValue(72, 0),
        // v just
        getIntValue(73, 0),
        // text
        getStringValue(1, ""),
        // style
        getStringValue(7, ""),
        // angle
        (getRealValue(50, 0.0)*2*M_PI)/360.0);

    creationInterface->addText(d);
}