void DL_Dxf::addCircle(DL_CreationInterface* creationInterface) {
    DL_CircleData d(getRealValue(10, 0.0),
                    getRealValue(20, 0.0),
                    getRealValue(30, 0.0),
                    getRealValue(40, 0.0));

    creationInterface->addCircle(d);
}