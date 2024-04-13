TEST_F(QueryPlannerTest, CompoundIndexBoundsDottedNotEqualsNull) {
    addIndex(BSON("a.b" << 1 << "c.d" << 1));
    runQuery(fromjson("{'a.b': {$gt: 'foo'}, 'c.d': {$ne: null}}"));

    assertNumSolutions(2U);
    assertSolutionExists("{cscan: {dir: 1}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {filter: null, pattern: "
        "{'a.b': 1, 'c.d': 1}, bounds: {'a.b': [['foo',{},false,false]], "
        "'c.d':[['MinKey',undefined,true,false],[null,'MaxKey',false,true]]}}}}}");
}