TEST_F(ExprMatchTest, ComparisonBetweenTwoFieldPathsMatchesCorrectly) {
    createMatcher(BSON("$expr" << BSON("$gt" << BSON_ARRAY("$a"
                                                           << "$b"))));

    ASSERT_TRUE(matches(BSON("a" << 10 << "b" << 2)));

    ASSERT_FALSE(matches(BSON("a" << 2 << "b" << 2)));
    ASSERT_FALSE(matches(BSON("a" << 2 << "b" << 10)));
}