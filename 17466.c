void DL_Dxf::writePolyline(DL_WriterA& dw,
                           const DL_PolylineData& data,
                           const DL_Attributes& attrib) {
    if (version==DL_VERSION_2000) {
        dw.entity("LWPOLYLINE");
        dw.dxfString(100, "AcDbEntity");
        dw.entityAttributes(attrib);
        dw.dxfString(100, "AcDbPolyline");
        dw.dxfInt(90, (int)data.number);
        dw.dxfInt(70, data.flags);
    } else {
        dw.entity("POLYLINE");
        dw.entityAttributes(attrib);
        polylineLayer = attrib.getLayer();
        dw.dxfInt(66, 1);
        dw.dxfInt(70, data.flags);
        dw.coord(DL_VERTEX_COORD_CODE, 0.0, 0.0, 0.0);
    }
}