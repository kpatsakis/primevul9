TEST_F(ExprMatchTest, SetCollatorChangesCollationUsedForComparisons) {
    createMatcher(fromjson("{$expr: {$eq: ['$x', 'abc']}}"));

    auto collator =
        stdx::make_unique<CollatorInterfaceMock>(CollatorInterfaceMock::MockType::kToLowerString);
    setCollator(collator.get());

    ASSERT_TRUE(matches(BSON("x"
                             << "AbC")));

    ASSERT_FALSE(matches(BSON("x"
                              << "cba")));
}