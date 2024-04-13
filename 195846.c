TEST_F(QueryPlannerTest, IntersectDisableAndHash) {
    bool oldEnableHashIntersection = internalQueryPlannerEnableHashIntersection.load();

    // Turn index intersection on but disable hash-based intersection.
    internalQueryPlannerEnableHashIntersection.store(false);
    params.options = QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::INDEX_INTERSECTION;

    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    addIndex(BSON("c" << 1));

    runQuery(fromjson("{a: {$gt: 1}, b: 1, c: 1}"));

    // We should do an AND_SORT intersection of {b: 1} and {c: 1}, but no AND_HASH plans.
    assertNumSolutions(4U);
    assertSolutionExists(
        "{fetch: {filter: {b: 1, c: 1}, node: {ixscan: "
        "{pattern: {a: 1}, bounds: {a: [[1,Infinity,false,true]]}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {a:{$gt:1},c:1}, node: {ixscan: "
        "{pattern: {b: 1}, bounds: {b: [[1,1,true,true]]}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {a:{$gt:1},b:1}, node: {ixscan: "
        "{pattern: {c: 1}, bounds: {c: [[1,1,true,true]]}}}}}");
    assertSolutionExists(
        "{fetch: {filter: {a:{$gt:1}, b: 1, c: 1}, node: {andSorted: {nodes: ["
        "{ixscan: {filter: null, pattern: {b:1}}},"
        "{ixscan: {filter: null, pattern: {c:1}}}]}}}}");

    // Restore the old value of the has intersection switch.
    internalQueryPlannerEnableHashIntersection.store(oldEnableHashIntersection);
}