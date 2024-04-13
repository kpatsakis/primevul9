void DL_Dxf::addDimAngular(DL_CreationInterface* creationInterface) {
    DL_DimensionData d = getDimData();

    // angular dimension:
    DL_DimAngular2LData da(
        // definition point 1
        getRealValue(13, 0.0),
        getRealValue(23, 0.0),
        getRealValue(33, 0.0),
        // definition point 2
        getRealValue(14, 0.0),
        getRealValue(24, 0.0),
        getRealValue(34, 0.0),
        // definition point 3
        getRealValue(15, 0.0),
        getRealValue(25, 0.0),
        getRealValue(35, 0.0),
        // definition point 4
        getRealValue(16, 0.0),
        getRealValue(26, 0.0),
        getRealValue(36, 0.0));
    creationInterface->addDimAngular(d, da);
}