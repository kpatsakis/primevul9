TEST_F(HeaderToMetadataTest, RejectBothCookieHeader) {
  const std::string config = R"EOF(
request_rules:
  - header: x-something
    cookie: something-else
    on_header_present:
      key: something
      value: else
      type: STRING
    remove: false

)EOF";
  auto expected = "Cannot specify both header and cookie";
  EXPECT_THROW_WITH_MESSAGE(initializeFilter(config), EnvoyException, expected);
}