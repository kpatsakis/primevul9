void DL_Dxf::addEllipse(DL_CreationInterface* creationInterface) {
    DL_EllipseData d(getRealValue(10, 0.0),
                     getRealValue(20, 0.0),
                     getRealValue(30, 0.0),
                     getRealValue(11, 0.0),
                     getRealValue(21, 0.0),
                     getRealValue(31, 0.0),
                     getRealValue(40, 1.0),
                     getRealValue(41, 0.0),
                     getRealValue(42, 2*M_PI));

    creationInterface->addEllipse(d);
}