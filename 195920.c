TEST_F(ExprMatchTest, FailGracefullyOnInvalidExpression) {
    ASSERT_THROWS_CODE(createMatcher(fromjson("{$expr: {$anyElementTrue: undefined}}")),
                       AssertionException,
                       17041);
    ASSERT_THROWS_CODE(
        createMatcher(fromjson("{$and: [{x: 1},{$expr: {$anyElementTrue: undefined}}]}")),
        AssertionException,
        17041);
    ASSERT_THROWS_CODE(
        createMatcher(fromjson("{$or: [{x: 1},{$expr: {$anyElementTrue: undefined}}]}")),
        AssertionException,
        17041);
    ASSERT_THROWS_CODE(
        createMatcher(fromjson("{$nor: [{x: 1},{$expr: {$anyElementTrue: undefined}}]}")),
        AssertionException,
        17041);
}