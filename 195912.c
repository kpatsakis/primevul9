TEST_F(ExprMatchTest, UndefinedMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$eq: ['$x', undefined]}}"));

    ASSERT_TRUE(matches(BSON("x" << BSONUndefined)));
    ASSERT_TRUE(matches(BSONObj()));

    ASSERT_FALSE(matches(BSON("x" << BSONNULL)));
}