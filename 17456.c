void DL_Dxf::addLayer(DL_CreationInterface* creationInterface) {
    // correct some invalid attributes for layers:
    attrib = creationInterface->getAttributes();
    if (attrib.getColor()==256 || attrib.getColor()==0) {
        attrib.setColor(7);
    }
    if (attrib.getWidth()<0) {
        attrib.setWidth(1);
    }

    std::string linetype = attrib.getLinetype();
    std::transform(linetype.begin(), linetype.end(), linetype.begin(), ::toupper);
    if (linetype=="BYLAYER" || linetype=="BYBLOCK") {
        attrib.setLinetype("CONTINUOUS");
    }

    // add layer
    std::string name = getStringValue(2, "");
    if (name.length()==0) {
        return;
    }

    creationInterface->addLayer(DL_LayerData(name, getIntValue(70, 0)));
}