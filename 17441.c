void DL_Dxf::addHatchEdge() {
    if (hatchEdge.defined) {
        if (hatchEdges.size()>0) {
            hatchEdges.back().push_back(hatchEdge);
        }
        hatchEdge = DL_HatchEdgeData();
    }
}