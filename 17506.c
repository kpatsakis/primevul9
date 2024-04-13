void DL_Dxf::addDimDiametric(DL_CreationInterface* creationInterface) {
    DL_DimensionData d = getDimData();

    // diametric dimension:
    DL_DimDiametricData dr(
        // definition point
        getRealValue(15, 0.0),
        getRealValue(25, 0.0),
        getRealValue(35, 0.0),
        // leader length:
        getRealValue(40, 0.0));
    creationInterface->addDimDiametric(d, dr);
}