TEST(HeaderMapImplTest, TestAppendHeader) {
  // Test appending to a string with a value.
  {
    TestRequestHeaderMapImpl headers;
    LowerCaseString foo("key1");
    headers.addCopy(foo, "some;");
    headers.appendCopy(foo, "test");
    EXPECT_EQ(headers.get(foo)->value().getStringView(), "some;,test");
  }

  // Test appending to an empty string.
  {
    TestRequestHeaderMapImpl headers;
    LowerCaseString key2("key2");
    headers.appendCopy(key2, "my tag data");
    EXPECT_EQ(headers.get(key2)->value().getStringView(), "my tag data");
  }

  // Test empty data case.
  {
    TestRequestHeaderMapImpl headers;
    LowerCaseString key3("key3");
    headers.addCopy(key3, "empty");
    headers.appendCopy(key3, "");
    EXPECT_EQ(headers.get(key3)->value().getStringView(), "empty");
  }
  // Regression test for appending to an empty string with a short string, then
  // setting integer.
  {
    TestRequestHeaderMapImpl headers;
    const std::string empty;
    headers.setPath(empty);
    // Append with default delimiter.
    headers.appendPath(" ", ",");
    headers.setPath(0);
    EXPECT_EQ("0", headers.getPathValue());
    EXPECT_EQ(1U, headers.Path()->value().size());
  }
  // Test append for inline headers using this method and append##name.
  {
    TestRequestHeaderMapImpl headers;
    headers.addCopy(Headers::get().Via, "1.0 fred");
    EXPECT_EQ(headers.getViaValue(), "1.0 fred");
    headers.appendCopy(Headers::get().Via, "1.1 p.example.net");
    EXPECT_EQ(headers.getViaValue(), "1.0 fred,1.1 p.example.net");
    headers.appendVia("1.1 new.example.net", ",");
    EXPECT_EQ(headers.getViaValue(), "1.0 fred,1.1 p.example.net,1.1 new.example.net");
  }
}