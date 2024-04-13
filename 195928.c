TEST_F(ExprMatchTest, ComparisonThrowsWithUnboundVariable) {
    ASSERT_THROWS(createMatcher(BSON("$expr" << BSON("$eq" << BSON_ARRAY("$a"
                                                                         << "$$var")))),
                  DBException);
}