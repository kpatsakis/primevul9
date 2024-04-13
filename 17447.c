bool DL_Dxf::in(std::istream& stream,
                DL_CreationInterface* creationInterface) {
    
    if (stream.good()) {
        firstCall=true;
        currentObjectType = DL_UNKNOWN;
        while (readDxfGroups(stream, creationInterface)) {}
        return true;
    }
    return false;
}