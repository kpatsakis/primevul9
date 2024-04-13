void DL_Dxf::addHatch(DL_CreationInterface* creationInterface) {
    DL_HatchData hd(getIntValue(91, 1),
                    getIntValue(70, 0),
                    getRealValue(41, 1.0),
                    getRealValue(52, 0.0),
                    getStringValue(2, ""));

    creationInterface->addHatch(hd);

    for (unsigned int i=0; i<hatchEdges.size(); i++) {
        creationInterface->addHatchLoop(DL_HatchLoopData(hatchEdges[i].size()));
        for (unsigned int k=0; k<hatchEdges[i].size(); k++) {
            creationInterface->addHatchEdge(DL_HatchEdgeData(hatchEdges[i][k]));
        }
    }

    creationInterface->endEntity();
}