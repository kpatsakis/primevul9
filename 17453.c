bool DL_Dxf::handleDictionaryData(DL_CreationInterface* creationInterface) {
    if (groupCode==3) {
        return true;
    }

    if (groupCode==5) {
        creationInterface->addDictionary(DL_DictionaryData(groupValue));
        return true;
    }

    if (groupCode==350) {
        creationInterface->addDictionaryEntry(DL_DictionaryEntryData(getStringValue(3, ""), groupValue));
        return true;
    }
    return false;
}