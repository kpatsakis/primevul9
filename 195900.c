TEST_F(ExprMatchTest, ConstantFalseValueExpressionMatchesCorrectly) {
    createMatcher(fromjson("{$expr: false}"));

    ASSERT_FALSE(matches(BSON("x" << 2)));
}