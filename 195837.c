TEST_F(QueryPlannerTest, IntersectBasicTwoPredCompound) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::INDEX_INTERSECTION;
    addIndex(BSON("a" << 1 << "c" << 1));
    addIndex(BSON("b" << 1));
    runQuery(fromjson("{a:1, b:1, c:1}"));

    // There's an andSorted not andHash because the two seeks are point intervals.
    assertSolutionExists(
        "{fetch: {filter: {a: 1, b: 1, c: 1}, node: {andSorted: {nodes: ["
        "{ixscan: {filter: null, pattern: {a:1, c:1}}},"
        "{ixscan: {filter: null, pattern: {b:1}}}]}}}}");
}