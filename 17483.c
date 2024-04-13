bool DL_Dxf::handleLeaderData(DL_CreationInterface* /*creationInterface*/) {
    // Allocate Leader vertices (group code 76):
    if (groupCode==76) {
        maxLeaderVertices = toInt(groupValue);
        if (maxLeaderVertices>0) {
            if (leaderVertices!=NULL) {
                delete[] leaderVertices;
            }
            leaderVertices = new double[3*maxLeaderVertices];
            for (int i=0; i<maxLeaderVertices; ++i) {
                leaderVertices[i*3] = 0.0;
                leaderVertices[i*3+1] = 0.0;
                leaderVertices[i*3+2] = 0.0;
            }
        }
        leaderVertexIndex=-1;
        return true;
    }

    // Process Leader vertices (group codes 10/20/30):
    else if (groupCode==10 || groupCode==20 || groupCode==30) {

        if (leaderVertexIndex<maxLeaderVertices-1 && groupCode==10) {
            leaderVertexIndex++;
        }

        if (groupCode<=30) {
            if (leaderVertexIndex>=0 &&
                    leaderVertexIndex<maxLeaderVertices) {
                leaderVertices[3*leaderVertexIndex + (groupCode/10-1)]
                = toReal(groupValue);
            }
        }
        return true;
    }

    return false;
}