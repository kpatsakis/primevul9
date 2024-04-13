TEST_F(ExprMatchTest, OrMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$or: [{$lte: ['$x', 3]}, {$gte: ['$y', 4]}]}}"));

    ASSERT_TRUE(matches(BSON("x" << 3)));
    ASSERT_TRUE(matches(BSON("y" << 5)));

    ASSERT_FALSE(matches(BSON("x" << 10)));
}