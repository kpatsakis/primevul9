void DL_Dxf::addPoint(DL_CreationInterface* creationInterface) {
    DL_PointData d(getRealValue(10, 0.0),
                   getRealValue(20, 0.0),
                   getRealValue(30, 0.0));
    creationInterface->addPoint(d);
}