TEST_F(QueryPlannerTest, IntersectBasicTwoPred) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::INDEX_INTERSECTION;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    runQuery(fromjson("{a:1, b:{$gt: 1}}"));

    assertSolutionExists(
        "{fetch: {filter: {a: 1, b: {$gt: 1}}, node: {andHash: {nodes: ["
        "{ixscan: {filter: null, pattern: {a:1}}},"
        "{ixscan: {filter: null, pattern: {b:1}}}]}}}}");
}