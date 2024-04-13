TEST_F(ExprMatchTest, ConstantTrueValueExpressionMatchesCorrectly) {
    createMatcher(fromjson("{$expr: true}"));

    ASSERT_TRUE(matches(BSON("x" << 2)));
}