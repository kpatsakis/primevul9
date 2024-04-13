TEST_F(QueryPlannerTest, IntersectManySelfIntersections) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::INDEX_INTERSECTION;
    // True means multikey.
    addIndex(BSON("a" << 1), true);

    // This one goes to 11.
    runQuery(fromjson("{a:1, a:2, a:3, a:4, a:5, a:6, a:7, a:8, a:9, a:10, a:11}"));

    // But this one only goes to 10.
    assertSolutionExists(
        "{fetch: {node: {andSorted: {nodes: ["
        "{ixscan: {filter: null, pattern: {a:1}}},"        // 1
        "{ixscan: {filter: null, pattern: {a:1}}},"        // 2
        "{ixscan: {filter: null, pattern: {a:1}}},"        // 3
        "{ixscan: {filter: null, pattern: {a:1}}},"        // 4
        "{ixscan: {filter: null, pattern: {a:1}}},"        // 5
        "{ixscan: {filter: null, pattern: {a:1}}},"        // 6
        "{ixscan: {filter: null, pattern: {a:1}}},"        // 7
        "{ixscan: {filter: null, pattern: {a:1}}},"        // 8
        "{ixscan: {filter: null, pattern: {a:1}}},"        // 9
        "{ixscan: {filter: null, pattern: {a:1}}}]}}}}");  // 10
}