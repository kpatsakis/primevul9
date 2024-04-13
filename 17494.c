void DL_Dxf::addLine(DL_CreationInterface* creationInterface) {
    DL_LineData d(getRealValue(10, 0.0),
                  getRealValue(20, 0.0),
                  getRealValue(30, 0.0),
                  getRealValue(11, 0.0),
                  getRealValue(21, 0.0),
                  getRealValue(31, 0.0));

    creationInterface->addLine(d);
}