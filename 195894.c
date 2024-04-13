TEST_F(QueryPlannerTest, CompoundIndexBoundsDottedNotEqualsNullWithProjection) {
    addIndex(BSON("a.b" << 1 << "c.d" << 1));
    runQuerySortProj(fromjson("{'a.b': {$gt: 'foo'}, 'c.d': {$ne: null}}"),
                     BSONObj(),
                     fromjson("{_id: 0, 'c.d': 1}"));

    assertNumSolutions(2U);
    assertSolutionExists("{proj: {spec: {_id: 0, 'c.d': 1}, node: {cscan: {dir: 1}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, 'c.d': 1}, node: {"
        "  ixscan: {filter: null, pattern: {'a.b': 1, 'c.d': 1}, bounds: {"
        "    'a.b': [['foo',{},false,false]], "
        "    'c.d':[['MinKey',undefined,true,false],[null,'MaxKey',false,true]]"
        "}}}}}");
}