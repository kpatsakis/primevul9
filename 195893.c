TEST_F(ExprMatchTest, NullMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$eq: ['$x', null]}}"));

    ASSERT_TRUE(matches(BSON("x" << BSONNULL)));

    ASSERT_FALSE(matches(BSON("x" << BSONUndefined)));
    ASSERT_FALSE(matches(BSONObj()));
}