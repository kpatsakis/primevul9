TEST_F(QueryPlannerTest, DottedFieldCovering) {
    addIndex(BSON("a.b" << 1));
    runQuerySortProj(fromjson("{'a.b': 5}"), BSONObj(), fromjson("{_id: 0, 'a.b': 1}"));

    ASSERT_EQUALS(getNumSolutions(), 2U);
    assertSolutionExists(
        "{proj: {spec: {_id: 0, 'a.b': 1}, node: "
        "{cscan: {dir: 1, filter: {'a.b': 5}}}}}");
    assertSolutionExists(
        "{proj: {spec: {_id: 0, 'a.b': 1}, node: {ixscan: {filter: null, pattern: {'a.b': 1},"
        "bounds: {'a.b': [[5,5,true,true]]}}}}}");
}