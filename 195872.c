TEST_F(ExprMatchTest, InWithDottedFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$in: ['$x.y', [1, 2, 3]]}}"));

    ASSERT_TRUE(matches(BSON("x" << BSON("y" << 3))));

    ASSERT_FALSE(matches(BSON("x" << BSON("y" << BSON_ARRAY(3)))));
}