TEST_F(ExprMatchTest, NestedAndWithTwoFieldPathsWithinOrMatchesCorrectly) {
    createMatcher(fromjson(
        "{$expr: {$or: [{$and: [{$eq: ['$x', '$w']}, {$eq: ['$z', 5]}]}, {$eq: ['$y', 4]}]}}"));

    ASSERT_TRUE(matches(BSON("x" << 2 << "w" << 2 << "z" << 5)));
    ASSERT_TRUE(matches(BSON("y" << 4)));

    ASSERT_FALSE(matches(BSON("x" << 2 << "w" << 4)));
    ASSERT_FALSE(matches(BSON("y" << 5)));
}