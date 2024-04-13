void DL_Dxf::writeHatchEdge(DL_WriterA& dw,
                            const DL_HatchEdgeData& data) {

    if (data.type<1 || data.type>4) {
        printf("WARNING: unsupported hatch edge type: %d", data.type);
    }

    dw.dxfInt(72, data.type);

    switch (data.type) {
    // line:
    case 1:
        dw.dxfReal(10, data.x1);
        dw.dxfReal(20, data.y1);
        dw.dxfReal(11, data.x2);
        dw.dxfReal(21, data.y2);
        break;

    // arc:
    case 2:
        dw.dxfReal(10, data.cx);
        dw.dxfReal(20, data.cy);
        dw.dxfReal(40, data.radius);
        dw.dxfReal(50, data.angle1/(2*M_PI)*360.0);
        dw.dxfReal(51, data.angle2/(2*M_PI)*360.0);
        dw.dxfInt(73, (int)(data.ccw));
        break;

    // ellipse arc:
    case 3:
        dw.dxfReal(10, data.cx);
        dw.dxfReal(20, data.cy);
        dw.dxfReal(11, data.mx);
        dw.dxfReal(21, data.my);
        dw.dxfReal(40, data.ratio);
        dw.dxfReal(50, data.angle1/(2*M_PI)*360.0);
        dw.dxfReal(51, data.angle2/(2*M_PI)*360.0);
        dw.dxfInt(73, (int)(data.ccw));
        break;

    // spline:
    case 4:
        dw.dxfInt(94, data.degree);
        dw.dxfBool(73, data.rational);
        dw.dxfBool(74, data.periodic);
        dw.dxfInt(95, data.nKnots);
        dw.dxfInt(96, data.nControl);
        for (unsigned int i=0; i<data.knots.size(); i++) {
            dw.dxfReal(40, data.knots[i]);
        }
        for (unsigned int i=0; i<data.controlPoints.size(); i++) {
            dw.dxfReal(10, data.controlPoints[i][0]);
            dw.dxfReal(20, data.controlPoints[i][1]);
        }
        for (unsigned int i=0; i<data.weights.size(); i++) {
            dw.dxfReal(42, data.weights[i]);
        }
        if (data.nFit>0) {
            dw.dxfInt(97, data.nFit);
            for (unsigned int i=0; i<data.fitPoints.size(); i++) {
                dw.dxfReal(11, data.fitPoints[i][0]);
                dw.dxfReal(21, data.fitPoints[i][1]);
            }
        }
        if (fabs(data.startTangentX)>1.0e-4 || fabs(data.startTangentY)>1.0e-4) {
            dw.dxfReal(12, data.startTangentX);
            dw.dxfReal(22, data.startTangentY);
        }
        if (fabs(data.endTangentX)>1.0e-4 || fabs(data.endTangentY)>1.0e-4) {
            dw.dxfReal(13, data.endTangentX);
            dw.dxfReal(23, data.endTangentY);
        }
        break;

    default:
        break;
    }
}