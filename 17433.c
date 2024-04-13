bool DL_Dxf::readDxfGroups(std::istream& stream,
                           DL_CreationInterface* creationInterface) {

    static int line = 1;

    // Read one group of the DXF file and chop the lines:
    if (DL_Dxf::getStrippedLine(groupCodeTmp, DL_DXF_MAXLINE, stream) &&
            DL_Dxf::getStrippedLine(groupValue, DL_DXF_MAXLINE, stream, false) ) {

        groupCode = (unsigned int)toInt(groupCodeTmp);

        line+=2;
        processDXFGroup(creationInterface, groupCode, groupValue);
    }
    return !stream.eof();
}