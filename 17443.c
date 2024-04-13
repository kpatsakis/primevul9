void DL_Dxf::addVertex(DL_CreationInterface* creationInterface) {

    // vertex defines a face of the mesh if its vertex flags group has the
    // 128 bit set but not the 64 bit. 10, 20, 30 are irrelevant and set to
    // 0 in this case
    if ((getIntValue(70, 0)&128) && !(getIntValue(70, 0)&64)) {
        return;
    }

    DL_VertexData d(getRealValue(10, 0.0),
                    getRealValue(20, 0.0),
                    getRealValue(30, 0.0),
                    getRealValue(42, 0.0));

    creationInterface->addVertex(d);
}