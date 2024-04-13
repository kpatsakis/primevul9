TEST(HeaderMapImplTest, RemoveIf) {
  LowerCaseString key1 = LowerCaseString("X-postfix-foo");
  LowerCaseString key2 = LowerCaseString("X-postfix-");
  LowerCaseString key3 = LowerCaseString("x-postfix-eep");

  TestRequestHeaderMapImpl headers;
  headers.addReference(key1, "value");
  headers.addReference(key2, "value");
  headers.addReference(key3, "value");

  EXPECT_EQ(0UL, headers.removeIf([](const HeaderEntry&) -> bool { return false; }));

  EXPECT_EQ(2UL, headers.removeIf([](const HeaderEntry& entry) -> bool {
    return absl::EndsWith(entry.key().getStringView(), "foo") ||
           absl::EndsWith(entry.key().getStringView(), "eep");
  }));

  TestRequestHeaderMapImpl expected{{"X-postfix-", "value"}};
  EXPECT_EQ(expected, headers);
}