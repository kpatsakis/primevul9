TEST_F(ExprMatchTest, ComparisonToConstantMatchesCorrectly) {
    createMatcher(BSON("$expr" << BSON("$eq" << BSON_ARRAY("$a" << 5))));

    ASSERT_TRUE(matches(BSON("a" << 5)));

    ASSERT_FALSE(matches(BSON("a" << 4)));
    ASSERT_FALSE(matches(BSON("a" << 6)));
}