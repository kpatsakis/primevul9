TEST_F(ExprMatchTest, AndWithNoMatchRewritableChildrenMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$and: [{$eq: ['$w', '$x']}, {$eq: ['$y', '$z']}]}}"));

    ASSERT_TRUE(matches(BSON("w" << 2 << "x" << 2 << "y" << 5 << "z" << 5)));

    ASSERT_FALSE(matches(BSON("w" << 1 << "x" << 2 << "y" << 5 << "z" << 5)));
    ASSERT_FALSE(matches(BSON("w" << 2 << "x" << 2 << "y" << 5 << "z" << 6)));
    ASSERT_FALSE(matches(BSON("w" << 2 << "y" << 5)));
}