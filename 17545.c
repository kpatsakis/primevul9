void DL_Dxf::addDimAligned(DL_CreationInterface* creationInterface) {
    DL_DimensionData d = getDimData();

    // aligned dimension:
    DL_DimAlignedData da(
        // extension point 1
        getRealValue(13, 0.0),
        getRealValue(23, 0.0),
        getRealValue(33, 0.0),
        // extension point 2
        getRealValue(14, 0.0),
        getRealValue(24, 0.0),
        getRealValue(34, 0.0));
    creationInterface->addDimAlign(d, da);
}