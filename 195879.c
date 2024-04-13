TEST_F(QueryPlannerTest, IndexBoundsOrOfNegations) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{$or: [{a: {$ne: null}}, {a: {$ne: 4}}]}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a:1}, "
        "bounds: {a: [['MinKey','MaxKey',true,true]]}}}}}");
}