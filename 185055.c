TEST(HeaderMapImplTest, TestInlineHeaderAdd) {
  TestRequestHeaderMapImpl foo;
  foo.addCopy(LowerCaseString(":path"), "GET");
  EXPECT_EQ(foo.size(), 1);
  EXPECT_TRUE(foo.Path() != nullptr);
}