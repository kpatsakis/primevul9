TEST_F(ExprMatchTest, OrNestedWithinAndMatchesCorrectly) {
    createMatcher(fromjson(
        "{$expr: {$and: [{$or: [{$eq: ['$x', 3]}, {$eq: ['$z', 5]}]}, {$eq: ['$y', 4]}]}}"));

    ASSERT_TRUE(matches(BSON("x" << 3 << "y" << 4)));
    ASSERT_TRUE(matches(BSON("z" << 5 << "y" << 4)));
    ASSERT_TRUE(matches(BSON("x" << 3 << "z" << 5 << "y" << 4)));

    ASSERT_FALSE(matches(BSON("x" << 3 << "z" << 5)));
    ASSERT_FALSE(matches(BSON("y" << 4)));
    ASSERT_FALSE(matches(BSON("x" << 3 << "y" << 10)));
}