TEST_F(QueryPlannerTest, ContainedOrNotPredicateIsLeadingFieldIndexIntersection) {
    params.options = QueryPlannerParams::INCLUDE_COLLSCAN | QueryPlannerParams::INDEX_INTERSECTION;
    addIndex(BSON("a" << 1 << "b" << 1));
    addIndex(BSON("c" << 1));

    runQuery(fromjson("{$and: [{$nor: [{a: 5}]}, {$or: [{b: 6}, {c: 7}]}]}"));
    assertNumSolutions(4);
    // The filter is {$not: {a: 5}}, but there is no way to write a BSON expression that will parse
    // to that MatchExpression.
    assertSolutionExists(
        "{fetch: {node: {or: {nodes: ["
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: {a: [['MinKey', 5, true, false], [5, 'MaxKey', "
        "false, true]], b: [[6, 6, true, "
        "true]]}}},"
        "{ixscan: {pattern: {c: 1}, bounds: {c: [[7, 7, true, true]]}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{b: 6}, {c: 7}]}, node: "
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: {a: [['MinKey', 5, true, false], [5, 'MaxKey', "
        "false, true]], b: [['MinKey', 'MaxKey', true, true]]}}}"
        "}}");
    assertSolutionExists(
        "{fetch: {filter: {$and:[{a: {$ne: 5}},{$or:[{a:{$ne:5},b:6},{c:7}]}]},"
        "node: {andHash: {nodes: ["
        "{or: {nodes: ["
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: {a: [['MinKey', 5, true, false], [5, 'MaxKey', "
        "false, true]], b: [[6, 6, true, true]]}}},"
        "{ixscan: {pattern: {c: 1}, bounds: {c: [[7, 7, true, true]]}}}]}},"
        "{ixscan: {pattern: {a: 1, b: 1}, bounds: {a: [['MinKey', 5, true, false], [5, 'MaxKey', "
        "false, true]], b: [['MinKey', 'MaxKey', true, true]]}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}