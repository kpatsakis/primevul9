bool DL_Dxf::handleMTextData(DL_CreationInterface* creationInterface) {
    // Special handling of text chunks for MTEXT entities:
    if (groupCode==3) {
        creationInterface->addMTextChunk(groupValue);
        return true;
    }

    return false;
}