TEST_F(QueryPlannerTest, MultipleContainedOrWithIndexIntersectionEnabled) {
    params.options = QueryPlannerParams::INCLUDE_COLLSCAN | QueryPlannerParams::INDEX_INTERSECTION;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1 << "a" << 1));
    addIndex(BSON("c" << 1));
    addIndex(BSON("d" << 1 << "a" << 1));
    addIndex(BSON("e" << 1));

    runQuery(fromjson("{$and: [{a: 5}, {$or: [{b: 6}, {c: 7}]}, {$or: [{d: 8}, {e: 9}]}]}"));

    assertNumSolutions(6U);

    // Non-ixisect solutions.
    assertSolutionExists(
        "{fetch: {filter: {$or: [{d: 8}, {e: 9}], a: 5}, node: {or: {nodes: ["
        "{ixscan: {filter: null, pattern: {b: 1, a: 1},"
        "bounds: {b: [[6,6,true,true]], a: [[5,5,true,true]]}}},"
        "{ixscan: {filter: null, pattern: {c: 1}, bounds: {c: [[7,7,true,true]]}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{b: 6}, {c: 7}], a: 5}, node: {or: {nodes: ["
        "{ixscan: {filter: null, pattern: {d: 1, a: 1},"
        "bounds: {d: [[8,8,true,true]], a: [[5,5,true,true]]}}},"
        "{ixscan: {filter: null, pattern: {e: 1}, bounds: {e: [[9,9,true,true]]}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$and: [{$or: [{b: 6}, {c: 7}]}, {$or: [{d: 8}, {e: 9}]}]}, node: "
        "{ixscan: {filter: null, pattern: {a: 1}, bounds: {a: [[5,5,true,true]]}}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");

    // Ixisect solutions.
    assertSolutionExists(
        "{fetch: {node: {andHash: {nodes: ["
        "{or: {nodes: ["
        "{ixscan: {filter: null, pattern: {b: 1, a: 1},"
        "bounds: {b: [[6,6,true,true]], a: [[5,5,true,true]]}}},"
        "{ixscan: {filter: null, pattern: {c: 1}, bounds: {c: [[7,7,true,true]]}}}"
        "]}},"
        "{ixscan: {filter: null, pattern: {a: 1}, bounds: {a: [[5,5,true,true]]}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {node: {andHash: {nodes: ["
        "{or: {nodes: ["
        "{ixscan: {filter: null, pattern: {d: 1, a: 1},"
        "bounds: {d: [[8,8,true,true]], a: [[5,5,true,true]]}}},"
        "{ixscan: {filter: null, pattern: {e: 1}, bounds: {e: [[9,9,true,true]]}}}"
        "]}},"
        "{ixscan: {filter: null, pattern: {a: 1}, bounds: {a: [[5,5,true,true]]}}}"
        "]}}}}");
}