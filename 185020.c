TEST(HeaderMapImplTest, ValidHeaderString) {
  EXPECT_TRUE(validHeaderString("abc"));
  EXPECT_FALSE(validHeaderString(absl::string_view("a\000bc", 4)));
  EXPECT_FALSE(validHeaderString("abc\n"));
}