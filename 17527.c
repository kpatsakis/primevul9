void DL_Dxf::addPolyline(DL_CreationInterface* creationInterface) {
    DL_PolylineData pd(maxVertices, getIntValue(71, 0), getIntValue(72, 0), getIntValue(70, 0), getRealValue(38, 0));
    creationInterface->addPolyline(pd);

    maxVertices = std::min(maxVertices, vertexIndex+1);

    if (currentObjectType==DL_ENTITY_LWPOLYLINE) {
        for (int i=0; i<maxVertices; i++) {
            DL_VertexData d(vertices[i*4],
                            vertices[i*4+1],
                            vertices[i*4+2],
                            vertices[i*4+3]);

            creationInterface->addVertex(d);
        }
        creationInterface->endEntity();
    }
}