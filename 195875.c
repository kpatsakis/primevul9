TEST_F(ExprMatchTest, EqWithTwoConstantsMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$eq: [3, 4]}}"));

    ASSERT_FALSE(matches(BSON("x" << 3)));
}