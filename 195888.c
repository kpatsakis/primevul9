TEST_F(QueryPlannerTest, ExprEqCanUseSparseIndexForEqualityToNull) {
    params.options &= ~QueryPlannerParams::INCLUDE_COLLSCAN;
    addIndex(fromjson("{a: 1}"), false, true);
    runQuery(fromjson("{a: {$_internalExprEq: null}}"));

    assertNumSolutions(1U);
    assertSolutionExists(
        "{fetch: {filter: {a: {$_internalExprEq: null}}, node: {ixscan: {filter: null, pattern: "
        "{a: 1}, bounds: {a: [[undefined,undefined,true,true], [null,null,true,true]]}}}}}");
}