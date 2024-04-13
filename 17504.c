bool DL_Dxf::handleLinetypeData(DL_CreationInterface* creationInterface) {
    if (groupCode == 49) {
        creationInterface->addLinetypeDash(toReal(groupValue));
        return true;
    }

    return false;
}