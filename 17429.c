void DL_Dxf::add3dFace(DL_CreationInterface* creationInterface) {
    DL_3dFaceData td;
    
    for (int k = 0; k < 4; k++) {
       td.x[k] = getRealValue(10 + k, 0.0);
       td.y[k] = getRealValue(20 + k, 0.0);
       td.z[k] = getRealValue(30 + k, 0.0);
    }
    creationInterface->add3dFace(td);
}