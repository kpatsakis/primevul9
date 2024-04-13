void DL_Dxf::addImageDef(DL_CreationInterface* creationInterface) {
    DL_ImageDefData id(// handle
        getStringValue(5, ""),
        getStringValue(1, ""));

    creationInterface->linkImage(id);
    creationInterface->endEntity();
    currentObjectType = DL_UNKNOWN;
}