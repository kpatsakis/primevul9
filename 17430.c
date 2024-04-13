void DL_Dxf::writeLeaderEnd(DL_WriterA& dw,
                 const DL_LeaderData& data) {
    if (version==DL_VERSION_2000) {
        dw.dxfString(1001, "ACAD");
        dw.dxfString(1000, "DSTYLE");
        dw.dxfString(1002, "{");
        dw.dxfInt(1070,40);
        dw.dxfReal(1040, data.dimScale);
        dw.dxfString(1002, "}");
    }
}