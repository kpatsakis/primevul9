TEST(HeaderMapImplTest, Equality) {
  TestRequestHeaderMapImpl headers1;
  TestRequestHeaderMapImpl headers2;
  EXPECT_EQ(headers1, headers2);

  headers1.addCopy(LowerCaseString("hello"), "world");
  EXPECT_FALSE(headers1 == headers2);

  headers2.addCopy(LowerCaseString("foo"), "bar");
  EXPECT_FALSE(headers1 == headers2);
}