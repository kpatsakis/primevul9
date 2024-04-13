TEST_F(QueryPlannerTest, NENullWithSortAndProjection) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(fromjson("{a: {$ne: null}}"), BSON("a" << 1), BSON("_id" << 0 << "a" << 1));

    assertNumSolutions(2U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: {sort: {pattern: {a: 1}, limit: 0, node: {sortKeyGen: "
        "{node: {cscan: {dir: 1}}}}}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: {"
        "  ixscan: {pattern: {a:1}, bounds: {"
        "    a: [['MinKey',undefined,true,false], [null,'MaxKey',false,true]]"
        "}}}}}");
}