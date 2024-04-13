bool DL_Dxf::handleXRecordData(DL_CreationInterface* creationInterface) {
    if (groupCode==105) {
        return false;
    }

    if (groupCode==5) {
        creationInterface->addXRecord(groupValue);
        return true;
    }

    if (groupCode==280) {
        xRecordValues = true;
        return true;
    }

    if (!xRecordValues) {
        return false;
    }

    // string:
    if (groupCode<=9 ||
        groupCode==100 || groupCode==102 || groupCode==105 ||
        (groupCode>=300 && groupCode<=369) ||
        (groupCode>=1000 && groupCode<=1009)) {

        creationInterface->addXRecordString(groupCode, groupValue);
        return true;
    }

    // int:
    else if ((groupCode>=60 && groupCode<=99) || (groupCode>=160 && groupCode<=179) || (groupCode>=270 && groupCode<=289)) {
        creationInterface->addXRecordInt(groupCode, toInt(groupValue));
        return true;
    }

    // bool:
    else if (groupCode>=290 && groupCode<=299) {
        creationInterface->addXRecordBool(groupCode, toBool(groupValue));
        return true;
    }

    // double:
    else if ((groupCode>=10 && groupCode<=59) || (groupCode>=110 && groupCode<=149) || (groupCode>=210 && groupCode<=239)) {
        creationInterface->addXRecordReal(groupCode, toReal(groupValue));
        return true;
    }

    return false;
}