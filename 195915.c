TEST_F(QueryPlannerTest, ContainedOrNotEqualsNull) {
    addIndex(BSON("b" << 1 << "a" << 1));
    addIndex(BSON("c" << 1 << "a" << 1));

    runQuery(fromjson("{$and: [{$nor: [{a: null}]}, {$or: [{b: 6}, {c: 7}]}]}"));
    assertNumSolutions(2);
    assertSolutionExists(
        "{fetch: {filter: null, node: {or: {nodes: ["
        "{ixscan: {pattern: {b: 1, a: 1}, bounds: {b: [[6, 6, true, true]], a: [['MinKey', "
        "undefined, true, false], [null, 'MaxKey', false, true]]}}},"
        "{ixscan: {pattern: {c: 1, a: 1}, bounds: {c: [[7, 7, true, true]], a: [['MinKey', "
        "undefined, true, false], [null, 'MaxKey', false, true]]}}}"
        "]}}}}");
    assertSolutionExists("{cscan: {dir: 1}}}}");
}