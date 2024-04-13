void DL_Dxf::addLinetype(DL_CreationInterface* creationInterface) {
    std::string name = getStringValue(2, "");
    if (name.length()==0) {
        return;
    }
    int numDashes = getIntValue(73, 0);
    //double dashes[numDashes];

    DL_LinetypeData d(
        // name:
        name,
        // description:
        getStringValue(3, ""),
        // flags
        getIntValue(70, 0),
        // number of dashes:
        numDashes,
        // pattern length:
        getRealValue(40, 0.0)
        // pattern:
        //dashes
    );

    if (name != "By Layer" && name != "By Block" && name != "BYLAYER" && name != "BYBLOCK") {
        creationInterface->addLinetype(d);
    }
}