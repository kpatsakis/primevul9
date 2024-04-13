void DL_Dxf::addArcAlignedText(DL_CreationInterface* creationInterface) {
    DL_ArcAlignedTextData d;
    d.text = getStringValue(1, "");
    d.font = getStringValue(2, "");
    d.style = getStringValue(7, "");
    d.cx = getRealValue(10, 0.0);
    d.cy = getRealValue(20, 0.0);
    d.cz = getRealValue(30, 0.0);
    d.radius = getRealValue(40, 0.0);
    d.xScaleFactor = getRealValue(41, 0.0);
    d.height = getRealValue(42, 0.0);
    d.spacing = getRealValue(43, 0.0);
    d.offset = getRealValue(44, 0.0);
    d.rightOffset = getRealValue(45, 0.0);
    d.leftOffset = getRealValue(46, 0.0);
    d.startAngle = getRealValue(50, 0.0);
    d.endAngle = getRealValue(51, 0.0);
    d.reversedCharacterOrder = getIntValue(70, 0);
    d.direction = getIntValue(71, 0);
    d.alignment = getIntValue(72, 0);
    d.side = getIntValue(73, 0);
    d.bold = getIntValue(74, 0);
    d.italic = getIntValue(75, 0);
    d.underline = getIntValue(76, 0);
    d.characerSet = getIntValue(77, 0);
    d.pitch = getIntValue(78, 0);
    d.shxFont = getIntValue(79, 0);
    d.wizard = getIntValue(280, 0);
    d.arcHandle = getIntValue(330, 0);

    creationInterface->addArcAlignedText(d);
}