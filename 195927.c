TEST_F(QueryPlannerTest, CannotTrimIxisectParamSelfIntersection) {
    params.options = QueryPlannerParams::INDEX_INTERSECTION;
    params.options |= QueryPlannerParams::NO_TABLE_SCAN;

    // true means multikey
    addIndex(BSON("a" << 1), true);

    runQuery(fromjson("{a: {$all: [1, 2, 3]}}"));

    assertNumSolutions(4U);
    assertSolutionExists(
        "{fetch: {filter: {$and: [{a:2}, {a:3}]}, node: "
        "{ixscan: {filter: null, pattern: {a: 1}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$and: [{a:1}, {a:3}]}, node: "
        "{ixscan: {filter: null, pattern: {a: 1}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$and: [{a:2}, {a:3}]}, node: "
        "{ixscan: {filter: null, pattern: {a: 1}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {a: {$all: [1, 2, 3]}}, node: {andSorted: {nodes: ["
        "{ixscan: {filter: null, pattern: {a:1},"
        "bounds: {a: [[1,1,true,true]]}}},"
        "{ixscan: {filter: null, pattern: {a:1},"
        "bounds: {a: [[2,2,true,true]]}}},"
        "{ixscan: {filter: null, pattern: {a:1},"
        "bounds: {a: [[3,3,true,true]]}}}]}}}}");
}