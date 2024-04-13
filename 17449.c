void DL_Dxf::writeLeaderVertex(DL_WriterA& dw,
                               const DL_LeaderVertexData& data) {
    if (version>DL_VERSION_R12) {
        dw.dxfReal(10, data.x);
        dw.dxfReal(20, data.y);
    }
}