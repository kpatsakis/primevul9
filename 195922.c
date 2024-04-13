TEST_F(QueryPlannerTest, CannotTrimIxisectAndHashWithOrChild) {
    params.options = QueryPlannerParams::INDEX_INTERSECTION;
    params.options |= QueryPlannerParams::NO_TABLE_SCAN;

    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    addIndex(BSON("c" << 1));

    runQuery(fromjson("{c: 1, $or: [{a: 1}, {b: 1, d: 1}]}"));

    assertNumSolutions(3U);

    assertSolutionExists(
        "{fetch: {filter: {c: 1}, node: {or: {nodes: ["
        "{fetch: {filter: {d: 1}, node: {ixscan: {filter: null,"
        "pattern: {b: 1}, bounds: {b: [[1,1,true,true]]}}}}},"
        "{ixscan: {filter: null, pattern: {a: 1},"
        "bounds: {a: [[1,1,true,true]]}}}]}}}}");

    assertSolutionExists(
        "{fetch: {filter: {$or:[{b:1,d:1},{a:1}]}, node:"
        "{ixscan: {filter: null, pattern: {c: 1}}}}}");

    assertSolutionExists(
        "{fetch: {filter: {c:1,$or:[{a:1},{b:1,d:1}]}, node:{andHash:{nodes:["
        "{or: {nodes: ["
        "{fetch: {filter: {d:1}, node: {ixscan: {pattern: {b: 1}}}}},"
        "{ixscan: {filter: null, pattern: {a: 1}}}]}},"
        "{ixscan: {filter: null, pattern: {c: 1}}}]}}}}");
}