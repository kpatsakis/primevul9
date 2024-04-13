TEST(HeaderMapImplTest, InlineAppend) {
  {
    TestRequestHeaderMapImpl headers;
    // Create via header and append.
    headers.setVia("");
    headers.appendVia("1.0 fred", ",");
    EXPECT_EQ(headers.getViaValue(), "1.0 fred");
    headers.appendVia("1.1 nowhere.com", ",");
    EXPECT_EQ(headers.getViaValue(), "1.0 fred,1.1 nowhere.com");
  }
  {
    // Append to via header without explicitly creating first.
    TestRequestHeaderMapImpl headers;
    headers.appendVia("1.0 fred", ",");
    EXPECT_EQ(headers.getViaValue(), "1.0 fred");
    headers.appendVia("1.1 nowhere.com", ",");
    EXPECT_EQ(headers.getViaValue(), "1.0 fred,1.1 nowhere.com");
  }
  {
    // Custom delimiter.
    TestRequestHeaderMapImpl headers;
    headers.setVia("");
    headers.appendVia("1.0 fred", ", ");
    EXPECT_EQ(headers.getViaValue(), "1.0 fred");
    headers.appendVia("1.1 nowhere.com", ", ");
    EXPECT_EQ(headers.getViaValue(), "1.0 fred, 1.1 nowhere.com");
  }
  {
    // Append and then later set.
    TestRequestHeaderMapImpl headers;
    headers.appendVia("1.0 fred", ",");
    headers.appendVia("1.1 nowhere.com", ",");
    EXPECT_EQ(headers.getViaValue(), "1.0 fred,1.1 nowhere.com");
    headers.setVia("2.0 override");
    EXPECT_EQ(headers.getViaValue(), "2.0 override");
  }
  {
    // Set and then append. This mimics how GrpcTimeout is set.
    TestRequestHeaderMapImpl headers;
    headers.setGrpcTimeout(42);
    EXPECT_EQ(headers.getGrpcTimeoutValue(), "42");
    headers.appendGrpcTimeout("s", "");
    EXPECT_EQ(headers.getGrpcTimeoutValue(), "42s");
  }
}