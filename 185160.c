TEST_F(HeaderUtilityTest, RemovePortsFromHostConnect) {
  const std::vector<std::pair<std::string, std::string>> host_headers{
      {"localhost:443", "localhost:443"},
  };
  for (const auto& host_pair : host_headers) {
    auto& host_header = hostHeaderEntry(host_pair.first, true);
    HeaderUtility::stripPortFromHost(headers_, 443);
    EXPECT_EQ(host_header.value().getStringView(), host_pair.second);
  }
}