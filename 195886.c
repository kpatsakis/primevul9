TEST_F(ExprMatchTest,
       OrWithAndContainingMatchRewritableAndNonMatchRewritableChildMatchesCorrectly) {
    createMatcher(fromjson(
        "{$expr: {$or: [{$eq: ['$x', 3]}, {$and: [{$eq: ['$y', 4]}, {$eq: ['$y', '$z']}]}]}}"));

    ASSERT_TRUE(matches(BSON("x" << 3)));
    ASSERT_TRUE(matches(BSON("y" << 4 << "z" << 4)));

    ASSERT_FALSE(matches(BSON("x" << 4)));
    ASSERT_FALSE(matches(BSON("y" << 4 << "z" << 5)));
}