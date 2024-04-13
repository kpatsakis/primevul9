bool DL_Dxf::handleSplineData(DL_CreationInterface* /*creationInterface*/) {
    // Allocate Spline knots (group code 72):
    if (groupCode==72) {
        maxKnots = toInt(groupValue);
        if (maxKnots>0) {
            if (knots!=NULL) {
                delete[] knots;
            }
            knots = new double[maxKnots];
            for (int i=0; i<maxKnots; ++i) {
                knots[i] = 0.0;
            }
        }
        knotIndex=-1;
        return true;
    }

    // Allocate Spline control points / weights (group code 73):
    else if (groupCode==73) {
        maxControlPoints = toInt(groupValue);
        if (maxControlPoints>0) {
            if (controlPoints!=NULL) {
                delete[] controlPoints;
            }
            if (weights!=NULL) {
                delete[] weights;
            }
            controlPoints = new double[3*maxControlPoints];
            weights = new double[maxControlPoints];
            for (int i=0; i<maxControlPoints; ++i) {
                controlPoints[i*3] = 0.0;
                controlPoints[i*3+1] = 0.0;
                controlPoints[i*3+2] = 0.0;
                weights[i] = 1.0;
            }
        }
        controlPointIndex=-1;
        weightIndex=-1;
        return true;
    }

    // Allocate Spline fit points (group code 74):
    else if (groupCode==74) {
        maxFitPoints = toInt(groupValue);
        if (maxFitPoints>0) {
            if (fitPoints!=NULL) {
                delete[] fitPoints;
            }
            fitPoints = new double[3*maxFitPoints];
            for (int i=0; i<maxFitPoints; ++i) {
                fitPoints[i*3] = 0.0;
                fitPoints[i*3+1] = 0.0;
                fitPoints[i*3+2] = 0.0;
            }
        }
        fitPointIndex=-1;
        return true;
    }

    // Process spline knot vertices (group code 40):
    else if (groupCode==40) {
        if (knotIndex<maxKnots-1) {
            knotIndex++;
            knots[knotIndex] = toReal(groupValue);
        }
        return true;
    }

    // Process spline control points (group codes 10/20/30):
    else if (groupCode==10 || groupCode==20 ||
             groupCode==30) {

        if (controlPointIndex<maxControlPoints-1 && groupCode==10) {
            controlPointIndex++;
        }

        if (controlPointIndex>=0 && controlPointIndex<maxControlPoints) {
            controlPoints[3*controlPointIndex + (groupCode/10-1)] = toReal(groupValue);
        }
        return true;
    }

    // Process spline fit points (group codes 11/21/31):
    else if (groupCode==11 || groupCode==21 || groupCode==31) {
        if (fitPointIndex<maxFitPoints-1 && groupCode==11) {
            fitPointIndex++;
        }

        if (fitPointIndex>=0 && fitPointIndex<maxFitPoints) {
            fitPoints[3*fitPointIndex + ((groupCode-1)/10-1)] = toReal(groupValue);
        }
        return true;
    }

    // Process spline weights (group code 41)
    else if (groupCode==41) {

        if (weightIndex<maxControlPoints-1) {
            weightIndex++;
        }

        if (weightIndex>=0 && weightIndex<maxControlPoints) {
            weights[weightIndex] = toReal(groupValue);
        }
        return true;
    }
    return false;
}