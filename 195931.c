TEST_F(QueryPlannerTest, IntersectSortFromAndHash) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::INDEX_INTERSECTION;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    runQuerySortProj(fromjson("{a: 1, b:{$gt: 1}}"), fromjson("{b:1}"), BSONObj());

    // This provides the sort.
    assertSolutionExists(
        "{fetch: {filter: {a: 1, b: {$gt: 1}}, node: {andHash: {nodes: ["
        "{ixscan: {filter: null, pattern: {a:1}}},"
        "{ixscan: {filter: null, pattern: {b:1}}}]}}}}");

    // Rearrange the preds, shouldn't matter.
    runQuerySortProj(fromjson("{b: 1, a:{$lt: 7}}"), fromjson("{b:1}"), BSONObj());
    assertSolutionExists(
        "{fetch: {filter: {b: 1, a: {$lt: 7}}, node: {andHash: {nodes: ["
        "{ixscan: {filter: null, pattern: {a:1}}},"
        "{ixscan: {filter: null, pattern: {b:1}}}]}}}}");
}