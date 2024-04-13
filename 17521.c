void DL_Dxf::writeVertex(DL_WriterA& dw,
                         const DL_VertexData& data) {


    if (version==DL_VERSION_2000) {
        dw.dxfReal(10, data.x);
        dw.dxfReal(20, data.y);
        if (fabs(data.bulge)>1.0e-10) {
            dw.dxfReal(42, data.bulge);
        }
    } else {
        dw.entity("VERTEX");
        //dw.entityAttributes(attrib);
        dw.dxfString(8, polylineLayer);
        dw.coord(DL_VERTEX_COORD_CODE, data.x, data.y, data.z);
        if (fabs(data.bulge)>1.0e-10) {
            dw.dxfReal(42, data.bulge);
        }
    }
}