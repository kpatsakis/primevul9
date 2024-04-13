void DL_Dxf::addDimRadial(DL_CreationInterface* creationInterface) {
    DL_DimensionData d = getDimData();

    DL_DimRadialData dr(
        // definition point
        getRealValue(15, 0.0),
        getRealValue(25, 0.0),
        getRealValue(35, 0.0),
        // leader length:
        getRealValue(40, 0.0));
    creationInterface->addDimRadial(d, dr);
}