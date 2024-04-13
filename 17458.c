void DL_Dxf::addXLine(DL_CreationInterface* creationInterface) {
    DL_XLineData d(getRealValue(10, 0.0),
                   getRealValue(20, 0.0),
                   getRealValue(30, 0.0),
                   getRealValue(11, 0.0),
                   getRealValue(21, 0.0),
                   getRealValue(31, 0.0));

    creationInterface->addXLine(d);
}