void DL_Dxf::addArc(DL_CreationInterface* creationInterface) {
    DL_ArcData d(getRealValue(10, 0.0),
                 getRealValue(20, 0.0),
                 getRealValue(30, 0.0),
                 getRealValue(40, 0.0),
                 getRealValue(50, 0.0),
                 getRealValue(51, 0.0));

    creationInterface->addArc(d);
}