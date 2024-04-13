bool DL_Dxf::readDxfGroups(FILE *fp, DL_CreationInterface* creationInterface) {

    static int line = 1;

    // Read one group of the DXF file and strip the lines:
    if (DL_Dxf::getStrippedLine(groupCodeTmp, DL_DXF_MAXLINE, fp) &&
            DL_Dxf::getStrippedLine(groupValue, DL_DXF_MAXLINE, fp, false) ) {

        groupCode = (unsigned int)toInt(groupCodeTmp);

        creationInterface->processCodeValuePair(groupCode, groupValue);
        line+=2;
        processDXFGroup(creationInterface, groupCode, groupValue);
    }

    return !feof(fp);
}