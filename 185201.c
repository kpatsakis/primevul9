TEST_F(HeaderUtilityTest, RemovePortsFromHost) {
  const std::vector<std::pair<std::string, std::string>> host_headers{
      {"localhost", "localhost"},           // w/o port part
      {"localhost:443", "localhost"},       // name w/ port
      {"", ""},                             // empty
      {":443", ""},                         // just port
      {"192.168.1.1", "192.168.1.1"},       // ipv4
      {"192.168.1.1:443", "192.168.1.1"},   // ipv4 w/ port
      {"[fc00::1]:443", "[fc00::1]"},       // ipv6 w/ port
      {"[fc00::1]", "[fc00::1]"},           // ipv6
      {":", ":"},                           // malformed string #1
      {"]:", "]:"},                         // malformed string #2
      {":abc", ":abc"},                     // malformed string #3
      {"localhost:80", "localhost:80"},     // port not matching w/ hostname
      {"192.168.1.1:80", "192.168.1.1:80"}, // port not matching w/ ipv4
      {"[fc00::1]:80", "[fc00::1]:80"}      // port not matching w/ ipv6
  };

  for (const auto& host_pair : host_headers) {
    auto& host_header = hostHeaderEntry(host_pair.first);
    HeaderUtility::stripPortFromHost(headers_, 443);
    EXPECT_EQ(host_header.value().getStringView(), host_pair.second);
  }
}