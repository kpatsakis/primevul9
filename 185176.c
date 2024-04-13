TEST_F(HeaderToMetadataTest, RejectInvalidRule) {
  const std::string config = R"EOF(
request_rules:
  - header: x-something
)EOF";
  auto expected = "header to metadata filter: rule for header 'x-something' has neither "
                  "`on_header_present` nor `on_header_missing` set";
  EXPECT_THROW_WITH_MESSAGE(initializeFilter(config), EnvoyException, expected);
}