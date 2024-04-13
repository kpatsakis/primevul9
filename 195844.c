TEST_F(ExprMatchTest, InitialCollationUsedForComparisons) {
    auto collator =
        stdx::make_unique<CollatorInterfaceMock>(CollatorInterfaceMock::MockType::kToLowerString);
    setCollator(collator.get());
    createMatcher(fromjson("{$expr: {$eq: ['$x', 'abc']}}"));

    ASSERT_TRUE(matches(BSON("x"
                             << "AbC")));

    ASSERT_FALSE(matches(BSON("x"
                              << "cba")));
}