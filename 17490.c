void DL_Dxf::addDictionary(DL_CreationInterface* creationInterface) {
    creationInterface->addDictionary(DL_DictionaryData(getStringValue(5, "")));
}