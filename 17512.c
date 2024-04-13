void DL_Dxf::addDimLinear(DL_CreationInterface* creationInterface) {
    DL_DimensionData d = getDimData();

    // horizontal / vertical / rotated dimension:
    DL_DimLinearData dl(
        // definition point 1
        getRealValue(13, 0.0),
        getRealValue(23, 0.0),
        getRealValue(33, 0.0),
        // definition point 2
        getRealValue(14, 0.0),
        getRealValue(24, 0.0),
        getRealValue(34, 0.0),
        // angle
        getRealValue(50, 0.0),
        // oblique
        getRealValue(52, 0.0));
    creationInterface->addDimLinear(d, dl);
}