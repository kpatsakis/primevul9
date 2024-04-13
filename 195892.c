TEST_F(ExprMatchTest, NeWithFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$ne: [3, '$x']}}"));

    ASSERT_TRUE(matches(BSON("x" << 1)));
    ASSERT_TRUE(matches(BSON("x" << 10)));

    ASSERT_FALSE(matches(BSON("x" << 3)));
}