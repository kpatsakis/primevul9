void DL_Dxf::addInsert(DL_CreationInterface* creationInterface) {
    //printf("addInsert\n");
    //printf("code 50: %s\n", values[50]);
    //printf("code 50 length: %d\n", strlen(values[50]));
    //printf("code 50:\n");
    //getRealValue(50, 0.0);

    std::string name = getStringValue(2, "");
    if (name.length()==0) {
        return;
    }

    DL_InsertData d(name,
                    // insertion point
                    getRealValue(10, 0.0),
                    getRealValue(20, 0.0),
                    getRealValue(30, 0.0),
                    // scale:
                    getRealValue(41, 1.0),
                    getRealValue(42, 1.0),
                    getRealValue(43, 1.0),
                    // angle (deg):
                    getRealValue(50, 0.0),
                    // cols / rows:
                    getIntValue(70, 1),
                    getIntValue(71, 1),
                    // spacing:
                    getRealValue(44, 0.0),
                    getRealValue(45, 0.0));

    creationInterface->addInsert(d);
}