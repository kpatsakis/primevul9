DL_DimensionData DL_Dxf::getDimData() {
    // generic dimension data:
    DL_DimensionData ret(
               // def point
               getRealValue(10, 0.0),
               getRealValue(20, 0.0),
               getRealValue(30, 0.0),
               // text middle point
               getRealValue(11, 0.0),
               getRealValue(21, 0.0),
               getRealValue(31, 0.0),
               // type
               getIntValue(70, 0),
               // attachment point
               getIntValue(71, 5),
               // line sp. style
               getIntValue(72, 1),
               // line sp. factor
               getRealValue(41, 1.0),
               // text
               getStringValue(1, ""),
               // style
               getStringValue(3, ""),
               // angle
               getRealValue(53, 0.0));
    ret.arrow1Flipped = getIntValue(74, 0)==1;
    ret.arrow2Flipped = getIntValue(75, 0)==1;
    return ret;
}