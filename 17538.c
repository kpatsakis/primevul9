void DL_Dxf::writeObjects(DL_WriterA& dw, const std::string& appDictionaryName) {
    dw.dxfString(  0, "SECTION");
    dw.dxfString(  2, "OBJECTS");

    dw.dxfString(  0, "DICTIONARY");
    dw.dxfHex(5, 0xC);
    dw.dxfString(100, "AcDbDictionary");
    dw.dxfInt(280, 0);
    dw.dxfInt(281, 1);
    dw.dxfString(  3, "ACAD_GROUP");
    dw.dxfHex(350, 0xD);
    dw.dxfString(  3, "ACAD_LAYOUT");
    dw.dxfHex(350, 0x1A);
    dw.dxfString(  3, "ACAD_MLINESTYLE");
    dw.dxfHex(350, 0x17);
    dw.dxfString(  3, "ACAD_PLOTSETTINGS");
    dw.dxfHex(350, 0x19);
    dw.dxfString(  3, "ACAD_PLOTSTYLENAME");
    dw.dxfHex(350, 0xE);
    dw.dxfString(  3, "AcDbVariableDictionary");
    int acDbVariableDictionaryHandle = dw.handle(350);
    //int acDbVariableDictionaryHandle = dw.getNextHandle();
    //dw.dxfHex(350, acDbVariableDictionaryHandle);
    //dw.incHandle();

    if (appDictionaryName.length()!=0) {
        dw.dxfString(  3, appDictionaryName);
        appDictionaryHandle = dw.handle(350);
        //appDictionaryHandle = dw.getNextHandle();
        //dw.dxfHex(350, appDictionaryHandle);
        //dw.incHandle();
    }

    dw.dxfString(  0, "DICTIONARY");
    dw.dxfHex(5, 0xD);
    //dw.handle();                                    // D
    //dw.dxfHex(330, 0xC);
    dw.dxfString(100, "AcDbDictionary");
    dw.dxfInt(280, 0);
    dw.dxfInt(281, 1);


    dw.dxfString(  0, "ACDBDICTIONARYWDFLT");
    dw.dxfHex(5, 0xE);
    //dicId4 = dw.handle();                           // E
    //dw.dxfHex(330, 0xC);                       // C
    dw.dxfString(100, "AcDbDictionary");
    dw.dxfInt(281, 1);
    dw.dxfString(  3, "Normal");
    dw.dxfHex(350, 0xF);
    //dw.dxfHex(350, dw.getNextHandle()+5);        // F
    dw.dxfString(100, "AcDbDictionaryWithDefault");
    dw.dxfHex(340, 0xF);
    //dw.dxfHex(340, dw.getNextHandle()+5);        // F


    dw.dxfString(  0, "ACDBPLACEHOLDER");
    dw.dxfHex(5, 0xF);
    //dw.handle();                                    // F
    //dw.dxfHex(330, dicId4);                      // E


    dw.dxfString(  0, "DICTIONARY");
    //dicId3 = dw.handle();                           // 17
    dw.dxfHex(5, 0x17);
    //dw.dxfHex(330, 0xC);                       // C
    dw.dxfString(100, "AcDbDictionary");
    dw.dxfInt(280, 0);
    dw.dxfInt(281, 1);
    dw.dxfString(  3, "Standard");
    dw.dxfHex(350, 0x18);
    //dw.dxfHex(350, dw.getNextHandle()+5);        // 18


    dw.dxfString(  0, "MLINESTYLE");
    dw.dxfHex(5, 0x18);
    //dw.handle();                                    // 18
    //dw.dxfHex(330, dicId3);                      // 17
    dw.dxfString(100, "AcDbMlineStyle");
    dw.dxfString(  2, "STANDARD");
    dw.dxfInt( 70, 0);
    dw.dxfString(  3, "");
    dw.dxfInt( 62, 256);
    dw.dxfReal( 51, 90.0);
    dw.dxfReal( 52, 90.0);
    dw.dxfInt( 71, 2);
    dw.dxfReal( 49, 0.5);
    dw.dxfInt( 62, 256);
    dw.dxfString(  6, "BYLAYER");
    dw.dxfReal( 49, -0.5);
    dw.dxfInt( 62, 256);
    dw.dxfString(  6, "BYLAYER");


    dw.dxfString(  0, "DICTIONARY");
    dw.dxfHex(5, 0x19);
    //dw.handle();                           // 17
    //dw.dxfHex(330, 0xC);                       // C
    dw.dxfString(100, "AcDbDictionary");
    dw.dxfInt(280, 0);
    dw.dxfInt(281, 1);


    dw.dxfString(  0, "DICTIONARY");
    //dicId2 = dw.handle();                           // 1A
    dw.dxfHex(5, 0x1A);
    //dw.dxfHex(330, 0xC);
    dw.dxfString(100, "AcDbDictionary");
    dw.dxfInt(281, 1);
    dw.dxfString(  3, "Layout1");
    dw.dxfHex(350, 0x1E);
    //dw.dxfHex(350, dw.getNextHandle()+2);        // 1E
    dw.dxfString(  3, "Layout2");
    dw.dxfHex(350, 0x26);
    //dw.dxfHex(350, dw.getNextHandle()+4);        // 26
    dw.dxfString(  3, "Model");
    dw.dxfHex(350, 0x22);
    //dw.dxfHex(350, dw.getNextHandle()+5);        // 22


    dw.dxfString(  0, "LAYOUT");
    dw.dxfHex(5, 0x1E);
    //dw.handle();                                    // 1E
    //dw.dxfHex(330, dicId2);                      // 1A
    dw.dxfString(100, "AcDbPlotSettings");
    dw.dxfString(  1, "");
    dw.dxfString(  2, "none_device");
    dw.dxfString(  4, "");
    dw.dxfString(  6, "");
    dw.dxfReal( 40, 0.0);
    dw.dxfReal( 41, 0.0);
    dw.dxfReal( 42, 0.0);
    dw.dxfReal( 43, 0.0);
    dw.dxfReal( 44, 0.0);
    dw.dxfReal( 45, 0.0);
    dw.dxfReal( 46, 0.0);
    dw.dxfReal( 47, 0.0);
    dw.dxfReal( 48, 0.0);
    dw.dxfReal( 49, 0.0);
    dw.dxfReal(140, 0.0);
    dw.dxfReal(141, 0.0);
    dw.dxfReal(142, 1.0);
    dw.dxfReal(143, 1.0);
    dw.dxfInt( 70, 688);
    dw.dxfInt( 72, 0);
    dw.dxfInt( 73, 0);
    dw.dxfInt( 74, 5);
    dw.dxfString(  7, "");
    dw.dxfInt( 75, 16);
    dw.dxfReal(147, 1.0);
    dw.dxfReal(148, 0.0);
    dw.dxfReal(149, 0.0);
    dw.dxfString(100, "AcDbLayout");
    dw.dxfString(  1, "Layout1");
    dw.dxfInt( 70, 1);
    dw.dxfInt( 71, 1);
    dw.dxfReal( 10, 0.0);
    dw.dxfReal( 20, 0.0);
    dw.dxfReal( 11, 420.0);
    dw.dxfReal( 21, 297.0);
    dw.dxfReal( 12, 0.0);
    dw.dxfReal( 22, 0.0);
    dw.dxfReal( 32, 0.0);
    dw.dxfReal( 14, 1.000000000000000E+20);
    dw.dxfReal( 24, 1.000000000000000E+20);
    dw.dxfReal( 34, 1.000000000000000E+20);
    dw.dxfReal( 15, -1.000000000000000E+20);
    dw.dxfReal( 25, -1.000000000000000E+20);
    dw.dxfReal( 35, -1.000000000000000E+20);
    dw.dxfReal(146, 0.0);
    dw.dxfReal( 13, 0.0);
    dw.dxfReal( 23, 0.0);
    dw.dxfReal( 33, 0.0);
    dw.dxfReal( 16, 1.0);
    dw.dxfReal( 26, 0.0);
    dw.dxfReal( 36, 0.0);
    dw.dxfReal( 17, 0.0);
    dw.dxfReal( 27, 1.0);
    dw.dxfReal( 37, 0.0);
    dw.dxfInt( 76, 0);
    //dw.dxfHex(330, dw.getPaperSpaceHandle());    // 1B
    dw.dxfHex(330, 0x1B);


    dw.dxfString(  0, "LAYOUT");
    dw.dxfHex(5, 0x22);
    //dw.handle();                                    // 22
    //dw.dxfHex(330, dicId2);                      // 1A
    dw.dxfString(100, "AcDbPlotSettings");
    dw.dxfString(  1, "");
    dw.dxfString(  2, "none_device");
    dw.dxfString(  4, "");
    dw.dxfString(  6, "");
    dw.dxfReal( 40, 0.0);
    dw.dxfReal( 41, 0.0);
    dw.dxfReal( 42, 0.0);
    dw.dxfReal( 43, 0.0);
    dw.dxfReal( 44, 0.0);
    dw.dxfReal( 45, 0.0);
    dw.dxfReal( 46, 0.0);
    dw.dxfReal( 47, 0.0);
    dw.dxfReal( 48, 0.0);
    dw.dxfReal( 49, 0.0);
    dw.dxfReal(140, 0.0);
    dw.dxfReal(141, 0.0);
    dw.dxfReal(142, 1.0);
    dw.dxfReal(143, 1.0);
    dw.dxfInt( 70, 1712);
    dw.dxfInt( 72, 0);
    dw.dxfInt( 73, 0);
    dw.dxfInt( 74, 0);
    dw.dxfString(  7, "");
    dw.dxfInt( 75, 0);
    dw.dxfReal(147, 1.0);
    dw.dxfReal(148, 0.0);
    dw.dxfReal(149, 0.0);
    dw.dxfString(100, "AcDbLayout");
    dw.dxfString(  1, "Model");
    dw.dxfInt( 70, 1);
    dw.dxfInt( 71, 0);
    dw.dxfReal( 10, 0.0);
    dw.dxfReal( 20, 0.0);
    dw.dxfReal( 11, 12.0);
    dw.dxfReal( 21, 9.0);
    dw.dxfReal( 12, 0.0);
    dw.dxfReal( 22, 0.0);
    dw.dxfReal( 32, 0.0);
    dw.dxfReal( 14, 0.0);
    dw.dxfReal( 24, 0.0);
    dw.dxfReal( 34, 0.0);
    dw.dxfReal( 15, 0.0);
    dw.dxfReal( 25, 0.0);
    dw.dxfReal( 35, 0.0);
    dw.dxfReal(146, 0.0);
    dw.dxfReal( 13, 0.0);
    dw.dxfReal( 23, 0.0);
    dw.dxfReal( 33, 0.0);
    dw.dxfReal( 16, 1.0);
    dw.dxfReal( 26, 0.0);
    dw.dxfReal( 36, 0.0);
    dw.dxfReal( 17, 0.0);
    dw.dxfReal( 27, 1.0);
    dw.dxfReal( 37, 0.0);
    dw.dxfInt( 76, 0);
    //dw.dxfHex(330, dw.getModelSpaceHandle());    // 1F
    dw.dxfHex(330, 0x1F);


    dw.dxfString(  0, "LAYOUT");
    //dw.handle();                                    // 26
    dw.dxfHex(5, 0x26);
    //dw.dxfHex(330, dicId2);                      // 1A
    dw.dxfString(100, "AcDbPlotSettings");
    dw.dxfString(  1, "");
    dw.dxfString(  2, "none_device");
    dw.dxfString(  4, "");
    dw.dxfString(  6, "");
    dw.dxfReal( 40, 0.0);
    dw.dxfReal( 41, 0.0);
    dw.dxfReal( 42, 0.0);
    dw.dxfReal( 43, 0.0);
    dw.dxfReal( 44, 0.0);
    dw.dxfReal( 45, 0.0);
    dw.dxfReal( 46, 0.0);
    dw.dxfReal( 47, 0.0);
    dw.dxfReal( 48, 0.0);
    dw.dxfReal( 49, 0.0);
    dw.dxfReal(140, 0.0);
    dw.dxfReal(141, 0.0);
    dw.dxfReal(142, 1.0);
    dw.dxfReal(143, 1.0);
    dw.dxfInt( 70, 688);
    dw.dxfInt( 72, 0);
    dw.dxfInt( 73, 0);
    dw.dxfInt( 74, 5);
    dw.dxfString(  7, "");
    dw.dxfInt( 75, 16);
    dw.dxfReal(147, 1.0);
    dw.dxfReal(148, 0.0);
    dw.dxfReal(149, 0.0);
    dw.dxfString(100, "AcDbLayout");
    dw.dxfString(  1, "Layout2");
    dw.dxfInt( 70, 1);
    dw.dxfInt( 71, 2);
    dw.dxfReal( 10, 0.0);
    dw.dxfReal( 20, 0.0);
    dw.dxfReal( 11, 12.0);
    dw.dxfReal( 21, 9.0);
    dw.dxfReal( 12, 0.0);
    dw.dxfReal( 22, 0.0);
    dw.dxfReal( 32, 0.0);
    dw.dxfReal( 14, 0.0);
    dw.dxfReal( 24, 0.0);
    dw.dxfReal( 34, 0.0);
    dw.dxfReal( 15, 0.0);
    dw.dxfReal( 25, 0.0);
    dw.dxfReal( 35, 0.0);
    dw.dxfReal(146, 0.0);
    dw.dxfReal( 13, 0.0);
    dw.dxfReal( 23, 0.0);
    dw.dxfReal( 33, 0.0);
    dw.dxfReal( 16, 1.0);
    dw.dxfReal( 26, 0.0);
    dw.dxfReal( 36, 0.0);
    dw.dxfReal( 17, 0.0);
    dw.dxfReal( 27, 1.0);
    dw.dxfReal( 37, 0.0);
    dw.dxfInt( 76, 0);
    //dw.dxfHex(330, dw.getPaperSpace0Handle());   // 23
    dw.dxfHex(330, 0x23);

    dw.dxfString(  0, "DICTIONARY");
    //dw.dxfHex(5, 0x2C);
    //dicId5 =
    dw.dxfHex(5, acDbVariableDictionaryHandle);
    //dw.handle();                           // 2C
    //dw.dxfHex(330, 0xC);                       // C
    dw.dxfString(100, "AcDbDictionary");
    dw.dxfInt(281, 1);
    dw.dxfString(  3, "DIMASSOC");
    //dw.dxfHex(350, 0x2F);
    dw.dxfHex(350, dw.getNextHandle()+1);        // 2E
    dw.dxfString(  3, "HIDETEXT");
    //dw.dxfHex(350, 0x2E);
    dw.dxfHex(350, dw.getNextHandle());        // 2D


    dw.dxfString(  0, "DICTIONARYVAR");
    //dw.dxfHex(5, 0x2E);
    dw.handle();                                    // 2E
    //dw.dxfHex(330, dicId5);                      // 2C
    dw.dxfString(100, "DictionaryVariables");
    dw.dxfInt(280, 0);
    dw.dxfInt(  1, 2);


    dw.dxfString(  0, "DICTIONARYVAR");
    //dw.dxfHex(5, 0x2D);
    dw.handle();                                    // 2D
    //dw.dxfHex(330, dicId5);                      // 2C
    dw.dxfString(100, "DictionaryVariables");
    dw.dxfInt(280, 0);
    dw.dxfInt(  1, 1);
}