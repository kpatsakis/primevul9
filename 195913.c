TEST_F(ExprMatchTest, NaNMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$eq: ['$x', NaN]}}"));

    ASSERT_TRUE(matches(BSON("x" << kNaN)));

    ASSERT_FALSE(matches(BSONObj()));
    ASSERT_FALSE(matches(BSON("x" << 0)));
    ASSERT_FALSE(matches(BSONObj()));

    createMatcher(fromjson("{$expr: {$lt: ['$x', NaN]}}"));

    ASSERT_TRUE(matches(BSONObj()));

    ASSERT_FALSE(matches(BSON("x" << kNaN)));
    ASSERT_FALSE(matches(BSON("x" << 0)));

    createMatcher(fromjson("{$expr: {$lte: ['$x', NaN]}}"));

    ASSERT_TRUE(matches(BSONObj()));
    ASSERT_TRUE(matches(BSON("x" << kNaN)));

    ASSERT_FALSE(matches(BSON("x" << 0)));

    createMatcher(fromjson("{$expr: {$gt: ['$x', NaN]}}"));

    ASSERT_TRUE(matches(BSON("x" << 0)));

    ASSERT_FALSE(matches(BSON("x" << kNaN)));
    ASSERT_FALSE(matches(BSONObj()));

    createMatcher(fromjson("{$expr: {$gte: ['$x', NaN]}}"));

    ASSERT_TRUE(matches(BSON("x" << 0)));
    ASSERT_TRUE(matches(BSON("x" << kNaN)));

    ASSERT_FALSE(matches(BSONObj()));
}