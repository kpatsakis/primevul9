void DL_Dxf::writeSpline(DL_WriterA& dw,
                         const DL_SplineData& data,
                         const DL_Attributes& attrib) {

    dw.entity("SPLINE");
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbSpline");
    }
    dw.dxfInt(70, data.flags);
    dw.dxfInt(71, data.degree);
    dw.dxfInt(72, data.nKnots);            // number of knots
    dw.dxfInt(73, data.nControl);          // number of control points
    dw.dxfInt(74, data.nFit);              // number of fit points
}