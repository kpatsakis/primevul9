TEST_F(ExprMatchTest, ComparisonToConstantVariableMatchesCorrectly) {
    setVariable("var", Value(5));
    createMatcher(BSON("$expr" << BSON("$eq" << BSON_ARRAY("$a"
                                                           << "$$var"))));

    ASSERT_TRUE(matches(BSON("a" << 5)));

    ASSERT_FALSE(matches(BSON("a" << 4)));
    ASSERT_FALSE(matches(BSON("a" << 6)));
}