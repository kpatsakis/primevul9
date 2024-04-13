TEST_F(ExprMatchTest, AndNestedWithinOrMatchesCorrectly) {
    createMatcher(fromjson(
        "{$expr: {$or: [{$and: [{$eq: ['$x', 3]}, {$gt: ['$z', 5]}]}, {$lt: ['$y', 4]}]}}"));

    ASSERT_TRUE(matches(BSON("x" << 3 << "z" << 7)));
    ASSERT_TRUE(matches(BSON("y" << 1)));

    ASSERT_FALSE(matches(BSON("y" << 5)));
}