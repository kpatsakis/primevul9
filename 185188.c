TEST_F(HeaderToMetadataTest, RejectRemoveForCookie) {
  const std::string config = R"EOF(
request_rules:
  - cookie: cookie
    on_header_present:
      metadata_namespace: envoy.lb
      key: version
      type: STRING
    remove: true
)EOF";
  auto expected = "Cannot specify remove for cookie";
  EXPECT_THROW_WITH_MESSAGE(initializeFilter(config), EnvoyException, expected);
}