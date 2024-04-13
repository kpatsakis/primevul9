TEST_F(ExprMatchTest, EqWithDottedFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$eq: ['$x.y', 3]}}"));

    ASSERT_TRUE(matches(BSON("x" << BSON("y" << 3))));

    ASSERT_FALSE(matches(BSON("x" << BSON("y" << BSON_ARRAY(3)))));
    ASSERT_FALSE(matches(BSON("x" << BSON_ARRAY(BSON("y" << 3)))));
    ASSERT_FALSE(matches(BSON("x" << BSON_ARRAY(BSON("y" << BSON_ARRAY(3))))));
}