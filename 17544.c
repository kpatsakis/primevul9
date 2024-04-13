bool DL_Dxf::handleHatchData(DL_CreationInterface* creationInterface) {
    // New polyline loop, group code 92
    // or new loop with individual edges, group code 93
    if (groupCode==92 || groupCode==93) {
        if (firstHatchLoop) {
            hatchEdges.clear();
            firstHatchLoop = false;
        }
        if (groupCode==92 && (toInt(groupValue)&2)==2) {
            addHatchLoop();
        }
        if (groupCode==93) {
            addHatchLoop();
        }
        return true;
    }

    // New hatch edge or new section / entity: add last hatch edge:
    if (groupCode==72 || groupCode==0 || groupCode==78 || groupCode==98) {
        // polyline boundaries use code 72 for bulge flag:
        if (groupCode!=72 || (getIntValue(92, 0)&2)==0) {
            addHatchEdge();
        }

        if (groupCode==0 /*|| groupCode==78*/) {
            addHatch(creationInterface);
        }
        else {
            hatchEdge.type = toInt(groupValue);
        }
        return true;
    }

    // polyline boundary:
    if ((getIntValue(92, 0)&2)==2) {
        switch (groupCode) {
        case 10:
            hatchEdge.type = 0;
            hatchEdge.vertices.push_back(std::vector<double>());
            hatchEdge.vertices.back().push_back(toReal(groupValue));
            return true;
        case 20:
            if (!hatchEdge.vertices.empty()) {
                hatchEdge.vertices.back().push_back(toReal(groupValue));
                hatchEdge.defined = true;
            }
            return true;
        case 42:
            if (!hatchEdge.vertices.empty()) {
                hatchEdge.vertices.back().push_back(toReal(groupValue));
                hatchEdge.defined = true;
            }
            return true;
        }
    }
    else {
        // Line edge:
        if (hatchEdge.type==1) {
            switch (groupCode) {
            case 10:
                hatchEdge.x1 = toReal(groupValue);
                return true;
            case 20:
                hatchEdge.y1 = toReal(groupValue);
                return true;
            case 11:
                hatchEdge.x2 = toReal(groupValue);
                return true;
            case 21:
                hatchEdge.y2 = toReal(groupValue);
                hatchEdge.defined = true;
                return true;
            }
        }

        // Arc edge:
        if (hatchEdge.type==2) {
            switch(groupCode) {
            case 10:
                hatchEdge.cx = toReal(groupValue);
                return true;
            case 20:
                hatchEdge.cy = toReal(groupValue);
                return true;
            case 40:
                hatchEdge.radius = toReal(groupValue);
                return true;
            case 50:
                hatchEdge.angle1 = toReal(groupValue)/360.0*2*M_PI;
                return true;
            case 51:
                hatchEdge.angle2 = toReal(groupValue)/360.0*2*M_PI;
                return true;
            case 73:
                hatchEdge.ccw = (bool)toInt(groupValue);
                hatchEdge.defined = true;
                return true;
            }
        }

        // Ellipse arc edge:
        if (hatchEdge.type==3) {
            switch (groupCode) {
            case 10:
                hatchEdge.cx = toReal(groupValue);
                return true;
            case 20:
                hatchEdge.cy = toReal(groupValue);
                return true;
            case 11:
                hatchEdge.mx = toReal(groupValue);
                return true;
            case 21:
                hatchEdge.my = toReal(groupValue);
                return true;
            case 40:
                hatchEdge.ratio = toReal(groupValue);
                return true;
            case 50:
                hatchEdge.angle1 = toReal(groupValue)/360.0*2*M_PI;
                return true;
            case 51:
                hatchEdge.angle2 = toReal(groupValue)/360.0*2*M_PI;
                return true;
            case 73:
                hatchEdge.ccw = (bool)toInt(groupValue);
                hatchEdge.defined = true;
                return true;
            }
        }

        // Spline edge:
        if (hatchEdge.type==4) {
            switch (groupCode) {
            case 94:
                hatchEdge.degree = toInt(groupValue);
                return true;
            case 73:
                hatchEdge.rational = toBool(groupValue);
                return true;
            case 74:
                hatchEdge.periodic = toBool(groupValue);
                return true;
            case 95:
                hatchEdge.nKnots = toInt(groupValue);
                return true;
            case 96:
                hatchEdge.nControl = toInt(groupValue);
                return true;
            case 97:
                hatchEdge.nFit = toInt(groupValue);
                return true;
            case 40:
                if (hatchEdge.knots.size() < hatchEdge.nKnots) {
                    hatchEdge.knots.push_back(toReal(groupValue));
                }
                return true;
            case 10:
                if (hatchEdge.controlPoints.size() < hatchEdge.nControl) {
                    std::vector<double> v;
                    v.push_back(toReal(groupValue));
                    hatchEdge.controlPoints.push_back(v);
                }
                return true;
            case 20:
                if (!hatchEdge.controlPoints.empty() && hatchEdge.controlPoints.back().size()==1) {
                    hatchEdge.controlPoints.back().push_back(toReal(groupValue));
                }
                hatchEdge.defined = true;
                return true;
            case 42:
                if (hatchEdge.weights.size() < hatchEdge.nControl) {
                    hatchEdge.weights.push_back(toReal(groupValue));
                }
                return true;
            case 11:
                if (hatchEdge.fitPoints.size() < hatchEdge.nFit) {
                    std::vector<double> v;
                    v.push_back(toReal(groupValue));
                    hatchEdge.fitPoints.push_back(v);
                }
                return true;
            case 21:
                if (!hatchEdge.fitPoints.empty() && hatchEdge.fitPoints.back().size()==1) {
                    hatchEdge.fitPoints.back().push_back(toReal(groupValue));
                }
                hatchEdge.defined = true;
                return true;
            case 12:
                hatchEdge.startTangentX = toReal(groupValue);
                return true;
            case 22:
                hatchEdge.startTangentY = toReal(groupValue);
                return true;
            case 13:
                hatchEdge.endTangentX = toReal(groupValue);
                return true;
            case 23:
                hatchEdge.endTangentY = toReal(groupValue);
                return true;
            }
        }
    }

    return false;
}