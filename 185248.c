  ipTestParamsToString(const ::testing::TestParamInfo<Network::Address::IpVersion>& params) {
    return params.param == Network::Address::IpVersion::v4 ? "IPv4" : "IPv6";
  }