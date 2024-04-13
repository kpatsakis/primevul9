bool DL_Dxf::handleXData(DL_CreationInterface* creationInterface) {
    if (groupCode==1001) {
        creationInterface->addXDataApp(groupValue);
        return true;
    }
    else if (groupCode>=1000 && groupCode<=1009) {
        creationInterface->addXDataString(groupCode, groupValue);
        return true;
    }
    else if (groupCode>=1010 && groupCode<=1059) {
        creationInterface->addXDataReal(groupCode, toReal(groupValue));
        return true;
    }
    else if (groupCode>=1060 && groupCode<=1070) {
        creationInterface->addXDataInt(groupCode, toInt(groupValue));
        return true;
    }
    else if (groupCode==1071) {
        creationInterface->addXDataInt(groupCode, toInt(groupValue));
        return true;
    }

    return false;
}