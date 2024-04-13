TEST(HeaderMapImplTest, HostHeader) {
  TestRequestHeaderMapImpl request_headers{{"host", "foo"}};
  EXPECT_EQ(request_headers.size(), 1);
  EXPECT_EQ(request_headers.get_(":authority"), "foo");

  TestRequestTrailerMapImpl request_trailers{{"host", "foo"}};
  EXPECT_EQ(request_trailers.size(), 1);
  EXPECT_EQ(request_trailers.get_("host"), "foo");

  TestResponseHeaderMapImpl response_headers{{"host", "foo"}};
  EXPECT_EQ(response_headers.size(), 1);
  EXPECT_EQ(response_headers.get_("host"), "foo");

  TestResponseTrailerMapImpl response_trailers{{"host", "foo"}};
  EXPECT_EQ(response_trailers.size(), 1);
  EXPECT_EQ(response_trailers.get_("host"), "foo");
}