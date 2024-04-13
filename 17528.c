void DL_Dxf::addHatchLoop() {
    addHatchEdge();
    hatchEdges.push_back(std::vector<DL_HatchEdgeData>());
}