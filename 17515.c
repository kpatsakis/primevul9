void DL_Dxf::addSpline(DL_CreationInterface* creationInterface) {
    DL_SplineData sd(getIntValue(71, 3), 
                     maxKnots, 
                     maxControlPoints, 
                     maxFitPoints,
                     getIntValue(70, 4));

    sd.tangentStartX = getRealValue(12, 0.0);
    sd.tangentStartY = getRealValue(22, 0.0);
    sd.tangentStartZ = getRealValue(32, 0.0);
    sd.tangentEndX = getRealValue(13, 0.0);
    sd.tangentEndY = getRealValue(23, 0.0);
    sd.tangentEndZ = getRealValue(33, 0.0);

    creationInterface->addSpline(sd);

    int i;
    for (i=0; i<maxControlPoints; i++) {
        DL_ControlPointData d(controlPoints[i*3],
                              controlPoints[i*3+1],
                              controlPoints[i*3+2], 
                              weights[i]);

        creationInterface->addControlPoint(d);
    }
    for (i=0; i<maxFitPoints; i++) {
        DL_FitPointData d(fitPoints[i*3],
                              fitPoints[i*3+1],
                              fitPoints[i*3+2]);

        creationInterface->addFitPoint(d);
    }
    for (i=0; i<maxKnots; i++) {
        DL_KnotData k(knots[i]);

        creationInterface->addKnot(k);
    }
    creationInterface->endEntity();
}