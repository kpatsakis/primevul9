TEST_F(QueryPlannerTest, ContainedOrPathLevelMultikeyCombineLeadingFields) {
    MultikeyPaths multikeyPaths{{}, {0U}};
    addIndex(BSON("a" << 1 << "c" << 1), multikeyPaths);
    addIndex(BSON("b" << 1));

    runQuery(fromjson("{$and: [{a: {$gte: 0}}, {$or: [{a: {$lte: 10}}, {b: 6}]}]}"));
    assertNumSolutions(3);
    assertSolutionExists(
        "{fetch: {filter: {a: {$gte: 0}}, node: {or: {nodes: ["
        "{ixscan: {pattern: {a: 1, c: 1}, bounds: {a: [[0, 10, true, true]], c: [['MinKey', "
        "'MaxKey', true, true]]}}},"
        "{ixscan: {pattern: {b: 1}, bounds: {b: [[6, 6, true, true]]}}}"
        "]}}}}");
    assertSolutionExists(
        "{fetch: {filter: {$or: [{a: {$lte: 10}}, {b: 6}]}, node: "
        "{ixscan: {pattern: {a: 1, c: 1}, bounds: {a: [[0, Infinity, true, true]], c: [['MinKey', "
        "'MaxKey', true, true]]}}}"
        "}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}