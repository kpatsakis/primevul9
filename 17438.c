void DL_Dxf::writeLeader(DL_WriterA& dw,
                         const DL_LeaderData& data,
                         const DL_Attributes& attrib) {
    if (version>DL_VERSION_R12) {
        dw.entity("LEADER");
        if (version==DL_VERSION_2000) {
            dw.dxfString(100, "AcDbEntity");
        }
        dw.entityAttributes(attrib);
        if (version==DL_VERSION_2000) {
            dw.dxfString(100, "AcDbLeader");
        }
        dw.dxfString(3, "Standard");
        dw.dxfInt(71, data.arrowHeadFlag);
        dw.dxfInt(72, data.leaderPathType);
        dw.dxfInt(73, data.leaderCreationFlag);
        dw.dxfInt(74, data.hooklineDirectionFlag);
        dw.dxfInt(75, data.hooklineFlag);
        dw.dxfReal(40, data.textAnnotationHeight);
        dw.dxfReal(41, data.textAnnotationWidth);
        dw.dxfInt(76, data.number);
    }
}