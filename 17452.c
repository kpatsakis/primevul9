void DL_Dxf::addSolid(DL_CreationInterface* creationInterface) {
    DL_SolidData sd;
    
    for (int k = 0; k < 4; k++) {
       sd.x[k] = getRealValue(10 + k, 0.0);
       sd.y[k] = getRealValue(20 + k, 0.0);
       sd.z[k] = getRealValue(30 + k, 0.0);
    }
    creationInterface->addSolid(sd);
}