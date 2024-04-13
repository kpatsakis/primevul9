TEST_F(ExprMatchTest, CmpMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$cmp: ['$x', 3]}}"));

    ASSERT_TRUE(matches(BSON("x" << 2)));
    ASSERT_TRUE(matches(BSON("x" << 4)));
    ASSERT_TRUE(matches(BSON("y" << 3)));

    ASSERT_FALSE(matches(BSON("x" << 3)));
}