TEST_F(QueryPlannerTest, NENull) {
    addIndex(BSON("a" << 1));
    runQuery(fromjson("{a: {$ne: null}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a:1}, bounds: {a: "
        "[['MinKey',undefined,true,false],[null,'MaxKey',false,true]]}}}}}");
}