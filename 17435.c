void DL_Dxf::writePoint(DL_WriterA& dw,
                        const DL_PointData& data,
                        const DL_Attributes& attrib) {
    dw.entity("POINT");
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbPoint");
    }
    dw.coord(DL_POINT_COORD_CODE, data.x, data.y, data.z);
}