TEST_F(ExprMatchTest, MatchAgainstArrayIsCorrect) {
    createMatcher(fromjson("{$expr: {$gt: ['$x', 4]}}"));

    // Matches because BSONType Array is greater than BSONType double.
    ASSERT_TRUE(matches(BSON("x" << BSON_ARRAY(1.0 << 2.0 << 3.0))));

    createMatcher(fromjson("{$expr: {$eq: ['$x', [4]]}}"));

    ASSERT_TRUE(matches(BSON("x" << BSON_ARRAY(4))));

    ASSERT_FALSE(matches(BSON("x" << 4)));
}