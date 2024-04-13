void DL_Dxf::addLeader(DL_CreationInterface* creationInterface) {
    // leader (arrow)
    DL_LeaderData le(
        // arrow head flag
        getIntValue(71, 1),
        // leader path type
        getIntValue(72, 0),
        // Leader creation flag
        getIntValue(73, 3),
        // Hookline direction flag
        getIntValue(74, 1),
        // Hookline flag
        getIntValue(75, 0),
        // Text annotation height
        getRealValue(40, 1.0),
        // Text annotation width
        getRealValue(41, 1.0),
        // Number of vertices in leader
        getIntValue(76, 0)
    );
    creationInterface->addLeader(le);

    for (int i=0; i<maxLeaderVertices; i++) {
        DL_LeaderVertexData d(leaderVertices[i*3],
                              leaderVertices[i*3+1],
                              leaderVertices[i*3+2]);

        creationInterface->addLeaderVertex(d);
    }
    creationInterface->endEntity();
}