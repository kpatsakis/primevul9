DL_Dxf::DL_Dxf() {
    version = DL_VERSION_2000;

    vertices = NULL;
    maxVertices = 0;
    vertexIndex = 0;

    knots = NULL;
    maxKnots = 0;
    knotIndex = 0;

    weights = NULL;
    weightIndex = 0;

    controlPoints = NULL;
    maxControlPoints = 0;
    controlPointIndex = 0;

    fitPoints = NULL;
    maxFitPoints = 0;
    fitPointIndex = 0;

    leaderVertices = NULL;
    maxLeaderVertices = 0;
    leaderVertexIndex = 0;
}