TEST_F(ExprMatchTest, ComplexExprMatchesCorrectly) {
    createMatcher(
        fromjson("{"
                 "  $expr: {"
                 "      $and: ["
                 "          {$eq: ['$a', 1]},"
                 "          {$eq: ['$b', '$c']},"
                 "          {"
                 "            $or: ["
                 "                {$eq: ['$d', 1]},"
                 "                {$eq: ['$e', 3]},"
                 "                {"
                 "                  $and: ["
                 "                      {$eq: ['$f', 1]},"
                 "                      {$eq: ['$g', '$h']},"
                 "                      {$or: [{$eq: ['$i', 3]}, {$eq: ['$j', '$k']}]}"
                 "                  ]"
                 "                }"
                 "            ]"
                 "          }"
                 "      ]"
                 "  }"
                 "}"));

    ASSERT_TRUE(matches(BSON("a" << 1 << "b" << 3 << "c" << 3 << "d" << 1)));
    ASSERT_TRUE(matches(BSON("a" << 1 << "b" << 3 << "c" << 3 << "e" << 3)));
    ASSERT_TRUE(matches(BSON("a" << 1 << "b" << 3 << "c" << 3 << "f" << 1 << "i" << 3)));
    ASSERT_TRUE(
        matches(BSON("a" << 1 << "b" << 3 << "c" << 3 << "f" << 1 << "j" << 5 << "k" << 5)));

    ASSERT_FALSE(matches(BSON("a" << 1)));
    ASSERT_FALSE(matches(BSON("a" << 1 << "b" << 3 << "c" << 3)));
    ASSERT_FALSE(matches(BSON("a" << 1 << "b" << 3 << "c" << 3 << "d" << 5)));
    ASSERT_FALSE(matches(BSON("a" << 1 << "b" << 3 << "c" << 3 << "j" << 5 << "k" << 10)));
}