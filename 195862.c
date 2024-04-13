TEST_F(QueryPlannerTest, NENullWithProjection) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(fromjson("{a: {$ne: null}}"), BSONObj(), BSON("_id" << 0 << "a" << 1));

    assertNumSolutions(2U);
    assertSolutionExists("{proj: {spec: {_id: 0, a: 1}, node: {cscan: {dir: 1}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, a: 1}, node: {ixscan: {pattern: {a:1}, "
        "bounds: {a: [['MinKey',undefined,true,false],[null,'MaxKey',false,true]]}}}}}");
}