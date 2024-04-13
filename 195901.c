TEST_F(QueryPlannerTest, CannotTrimIxisectParamBeneathOr) {
    params.options = QueryPlannerParams::INDEX_INTERSECTION;
    params.options |= QueryPlannerParams::NO_TABLE_SCAN;

    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    addIndex(BSON("c" << 1));

    runQuery(fromjson("{d: 1, $or: [{a: 1}, {b: 1, c: 1}]}"));

    assertNumSolutions(3U);

    assertSolutionExists(
        "{fetch: {filter: {d: 1}, node: {or: {nodes: ["
        "{fetch: {filter: {c: 1}, node: {ixscan: {filter: null,"
        "pattern: {b: 1}, bounds: {b: [[1,1,true,true]]}}}}},"
        "{ixscan: {filter: null, pattern: {a: 1},"
        "bounds: {a: [[1,1,true,true]]}}}]}}}}");

    assertSolutionExists(
        "{fetch: {filter: {d: 1}, node: {or: {nodes: ["
        "{fetch: {filter: {b: 1}, node: {ixscan: {filter: null,"
        "pattern: {c: 1}, bounds: {c: [[1,1,true,true]]}}}}},"
        "{ixscan: {filter: null, pattern: {a: 1},"
        "bounds: {a: [[1,1,true,true]]}}}]}}}}");

    assertSolutionExists(
        "{fetch: {filter: {d: 1}, node: {or: {nodes: ["
        "{fetch: {filter: {b: 1, c: 1}, node: {andSorted: {nodes: ["
        "{ixscan: {filter: null, pattern: {b: 1}}},"
        "{ixscan: {filter: null, pattern: {c: 1}}}]}}}},"
        "{ixscan: {filter: null, pattern: {a: 1}}}]}}}}");
}