TEST_F(ExprMatchTest, ExprLtDoesNotUseTypeBracketing) {
    createMatcher(fromjson("{$expr: {$lt: ['$x', true]}}"));

    ASSERT_TRUE(matches(BSON("x" << false)));
    ASSERT_TRUE(matches(BSON("x" << BSON("y" << 1))));
    ASSERT_TRUE(matches(BSONObj()));

    ASSERT_FALSE(matches(BSON("x" << Timestamp(0, 1))));
}