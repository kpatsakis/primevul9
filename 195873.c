TEST_F(ExprMatchTest, AndMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$and: [{$eq: ['$x', 3]}, {$ne: ['$y', 4]}]}}"));

    ASSERT_TRUE(matches(BSON("x" << 3)));
    ASSERT_TRUE(matches(BSON("x" << 3 << "y" << 5)));

    ASSERT_FALSE(matches(BSON("x" << 10 << "y" << 5)));
    ASSERT_FALSE(matches(BSON("x" << 3 << "y" << 4)));
    ASSERT_FALSE(matches(BSON("x" << 10 << "y" << 5)));
}