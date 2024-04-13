TEST_F(QueryPlannerTest, ContainedOrPredicateIsLeadingFieldInBothBranchesIndexIntersection) {
    params.options = QueryPlannerParams::INCLUDE_COLLSCAN | QueryPlannerParams::INDEX_INTERSECTION;
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("a" << 1 << "c" << 1));

    runQuery(fromjson("{$and: [{a: 5}, {$or: [{b: 6}, {c: 7}]}]}"));
    assertNumSolutions(6);
    assertSolutionExists(
        "{fetch: {node: {or: {nodes: ["
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: {a: [[5, 5, true, true]], b: [[6, 6, true, "
        "true]]}}},"
        "{ixscan: {pattern: {a: 1, c: 1}, bounds: {a: [[5, 5, true, true]], c: [[7, 7, true, "
        "true]]}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{b: 6}, {c: 7}]}, node: "
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: {a: [[5, 5, true, true]], b: [['MinKey', "
        "'MaxKey', true, true]]}}}"
        "}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{b: 6}, {c: 7}]}, node: "
        "{ixscan: {pattern: {a: 1, c: 1}, bounds: {a: [[5, 5, true, true]], c: [['MinKey', "
        "'MaxKey', true, true]]}}}"
        "}}");
    // The AND_HASH stage is not really needed, since the predicate {a: 5} is covered by the indexed
    // OR.
    assertSolutionExists(
        "{fetch: {filter: {$and:[{a:5},{$or:[{a:5,b:6},{a:5,c:7}]}]}, node: {andHash: {nodes: ["
        "{or: {nodes: ["
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: {a: [[5, 5, true, true]], b: [[6, 6, true, "
        "true]]}}},"
        "{ixscan: {pattern: {a: 1, c: 1}, bounds: {a: [[5, 5, true, true]], c: [[7, 7, true, "
        "true]]}}}]}},"
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: {a: [[5, 5, true, true]], b: [['MinKey', "
        "'MaxKey', true, true]]}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$and:[{a:5},{$or:[{a:5,b:6},{a:5,c:7}]}]}, node: {andHash: {nodes: ["
        "{or: {nodes: ["
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: {a: [[5, 5, true, true]], b: [[6, 6, true, "
        "true]]}}},"
        "{ixscan: {pattern: {a: 1, c: 1}, bounds: {a: [[5, 5, true, true]], c: [[7, 7, true, "
        "true]]}}}]}},"
        "{ixscan: {pattern: {a: 1, c: 1}, bounds: {a: [[5, 5, true, true]], c: [['MinKey', "
        "'MaxKey', true, true]]}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}