void DL_Dxf::writeInsert(DL_WriterA& dw,
                         const DL_InsertData& data,
                         const DL_Attributes& attrib) {

    if (data.name.empty()) {
        std::cerr << "DL_Dxf::writeInsert: "
        << "Block name must not be empty\n";
        return;
    }

    dw.entity("INSERT");
    if (version==DL_VERSION_2000) {
        dw.dxfString(100, "AcDbEntity");
    }
    dw.entityAttributes(attrib);
    if (version==DL_VERSION_2000) {
        if (data.cols!=1 || data.rows!=1) {
            dw.dxfString(100, "AcDbMInsertBlock");
        }
        else {
            dw.dxfString(100, "AcDbBlockReference");
        }
    }
    dw.dxfString(2, data.name);
    dw.dxfReal(10, data.ipx);
    dw.dxfReal(20, data.ipy);
    dw.dxfReal(30, data.ipz);
    if (data.sx!=1.0 || data.sy!=1.0) {
        dw.dxfReal(41, data.sx);
        dw.dxfReal(42, data.sy);
        dw.dxfReal(43, 1.0);
    }
    if (data.angle!=0.0) {
        dw.dxfReal(50, data.angle);
    }
    if (data.cols!=1 || data.rows!=1) {
        dw.dxfInt(70, data.cols);
        dw.dxfInt(71, data.rows);
    }
    if (data.colSp!=0.0 || data.rowSp!=0.0) {
        dw.dxfReal(44, data.colSp);
        dw.dxfReal(45, data.rowSp);
    }
}