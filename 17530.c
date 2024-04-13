DL_Dxf::~DL_Dxf() {
    if (vertices!=NULL) {
        delete[] vertices;
    }
    if (knots!=NULL) {
        delete[] knots;
    }
    if (controlPoints!=NULL) {
        delete[] controlPoints;
    }
    if (fitPoints!=NULL) {
        delete[] fitPoints;
    }
    if (weights!=NULL) {
        delete[] weights;
    }
    if (leaderVertices!=NULL) {
        delete[] leaderVertices;
    }
}