TEST_F(ExprMatchTest, ConstantLiteralExpressionMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$literal: {$eq: ['$x', 10]}}}"));

    ASSERT_TRUE(matches(BSON("x" << 2)));
}