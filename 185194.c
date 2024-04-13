TEST_F(HeaderToMetadataTest, RejectEmptyHeader) {
  const std::string config = R"EOF(
request_rules:
  - header: ""

)EOF";
  auto expected = "One of Cookie or Header option needs to be specified";
  EXPECT_THROW_WITH_MESSAGE(initializeFilter(config), EnvoyException, expected);
}