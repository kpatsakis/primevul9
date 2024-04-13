TEST_F(QueryPlannerTest, NENullWithSort) {
    addIndex(BSON("a" << 1));
    runQuerySortProj(fromjson("{a: {$ne: null}}"), BSON("a" << 1), BSONObj());

    assertNumSolutions(2U);
    assertSolutionExists(
        "{sort: {"
        "  pattern: {a: 1},"
        "  limit: 0,"
        "  node: {"
        "    sortKeyGen: {"
        "      node: {"
        "        cscan: {"
        "          filter: {a: {$ne: null}}, "
        "          dir: 1"
        "        }}}}}}");
    assertSolutionExists(
        "{fetch: {filter: null, node: {ixscan: {pattern: {a:1}, "
        "bounds: {a: [['MinKey',undefined,true,false],"
        "[null,'MaxKey',false,true]]}}}}}");
}