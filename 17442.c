void DL_Dxf::addDictionaryEntry(DL_CreationInterface* creationInterface) {
    creationInterface->addDictionaryEntry(DL_DictionaryEntryData(getStringValue(3, ""), getStringValue(350, "")));
}