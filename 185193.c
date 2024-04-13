TEST(Context, ConnectionAttributes) {
  NiceMock<StreamInfo::MockStreamInfo> info;
  std::shared_ptr<NiceMock<Envoy::Upstream::MockHostDescription>> upstream_host(
      new NiceMock<Envoy::Upstream::MockHostDescription>());
  auto downstream_ssl_info = std::make_shared<NiceMock<Ssl::MockConnectionInfo>>();
  auto upstream_ssl_info = std::make_shared<NiceMock<Ssl::MockConnectionInfo>>();
  ConnectionWrapper connection(info);
  UpstreamWrapper upstream(info);
  PeerWrapper source(info, false);
  PeerWrapper destination(info, true);

  Network::Address::InstanceConstSharedPtr local =
      Network::Utility::parseInternetAddress("1.2.3.4", 123, false);
  Network::Address::InstanceConstSharedPtr remote =
      Network::Utility::parseInternetAddress("10.20.30.40", 456, false);
  Network::Address::InstanceConstSharedPtr upstream_address =
      Network::Utility::parseInternetAddress("10.1.2.3", 679, false);
  Network::Address::InstanceConstSharedPtr upstream_local_address =
      Network::Utility::parseInternetAddress("10.1.2.3", 1000, false);
  const std::string sni_name = "kittens.com";
  EXPECT_CALL(info, downstreamLocalAddress()).WillRepeatedly(ReturnRef(local));
  EXPECT_CALL(info, downstreamRemoteAddress()).WillRepeatedly(ReturnRef(remote));
  EXPECT_CALL(info, downstreamSslConnection()).WillRepeatedly(Return(downstream_ssl_info));
  EXPECT_CALL(info, upstreamSslConnection()).WillRepeatedly(Return(upstream_ssl_info));
  EXPECT_CALL(info, upstreamHost()).WillRepeatedly(Return(upstream_host));
  EXPECT_CALL(info, requestedServerName()).WillRepeatedly(ReturnRef(sni_name));
  EXPECT_CALL(info, upstreamLocalAddress()).WillRepeatedly(ReturnRef(upstream_local_address));
  const std::string upstream_transport_failure_reason = "ConnectionTermination";
  EXPECT_CALL(info, upstreamTransportFailureReason())
      .WillRepeatedly(ReturnRef(upstream_transport_failure_reason));
  EXPECT_CALL(*downstream_ssl_info, peerCertificatePresented()).WillRepeatedly(Return(true));
  EXPECT_CALL(*upstream_host, address()).WillRepeatedly(Return(upstream_address));

  const std::string tls_version = "TLSv1";
  EXPECT_CALL(*downstream_ssl_info, tlsVersion()).WillRepeatedly(ReturnRef(tls_version));
  EXPECT_CALL(*upstream_ssl_info, tlsVersion()).WillRepeatedly(ReturnRef(tls_version));
  std::vector<std::string> dns_sans_peer = {"www.peer.com"};
  EXPECT_CALL(*downstream_ssl_info, dnsSansPeerCertificate()).WillRepeatedly(Return(dns_sans_peer));
  EXPECT_CALL(*upstream_ssl_info, dnsSansPeerCertificate()).WillRepeatedly(Return(dns_sans_peer));
  std::vector<std::string> dns_sans_local = {"www.local.com"};
  EXPECT_CALL(*downstream_ssl_info, dnsSansLocalCertificate())
      .WillRepeatedly(Return(dns_sans_local));
  EXPECT_CALL(*upstream_ssl_info, dnsSansLocalCertificate()).WillRepeatedly(Return(dns_sans_local));
  std::vector<std::string> uri_sans_peer = {"www.peer.com/uri"};
  EXPECT_CALL(*downstream_ssl_info, uriSanPeerCertificate()).WillRepeatedly(Return(uri_sans_peer));
  EXPECT_CALL(*upstream_ssl_info, uriSanPeerCertificate()).WillRepeatedly(Return(uri_sans_peer));
  std::vector<std::string> uri_sans_local = {"www.local.com/uri"};
  EXPECT_CALL(*downstream_ssl_info, uriSanLocalCertificate())
      .WillRepeatedly(Return(uri_sans_local));
  EXPECT_CALL(*upstream_ssl_info, uriSanLocalCertificate()).WillRepeatedly(Return(uri_sans_local));
  const std::string subject_local = "local.com";
  EXPECT_CALL(*downstream_ssl_info, subjectLocalCertificate())
      .WillRepeatedly(ReturnRef(subject_local));
  EXPECT_CALL(*upstream_ssl_info, subjectLocalCertificate())
      .WillRepeatedly(ReturnRef(subject_local));
  const std::string subject_peer = "peer.com";
  EXPECT_CALL(*downstream_ssl_info, subjectPeerCertificate())
      .WillRepeatedly(ReturnRef(subject_peer));
  EXPECT_CALL(*upstream_ssl_info, subjectPeerCertificate()).WillRepeatedly(ReturnRef(subject_peer));

  {
    auto value = connection[CelValue::CreateStringView(Undefined)];
    EXPECT_FALSE(value.has_value());
  }

  {
    auto value = connection[CelValue::CreateInt64(13)];
    EXPECT_FALSE(value.has_value());
  }

  {
    auto value = source[CelValue::CreateStringView(Undefined)];
    EXPECT_FALSE(value.has_value());
  }

  {
    auto value = source[CelValue::CreateInt64(13)];
    EXPECT_FALSE(value.has_value());
  }

  {
    auto value = destination[CelValue::CreateStringView(Address)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ("1.2.3.4:123", value.value().StringOrDie().value());
  }

  {
    auto value = destination[CelValue::CreateStringView(Port)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsInt64());
    EXPECT_EQ(123, value.value().Int64OrDie());
  }

  {
    auto value = source[CelValue::CreateStringView(Address)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ("10.20.30.40:456", value.value().StringOrDie().value());
  }

  {
    auto value = source[CelValue::CreateStringView(Port)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsInt64());
    EXPECT_EQ(456, value.value().Int64OrDie());
  }

  {
    auto value = upstream[CelValue::CreateStringView(Address)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ("10.1.2.3:679", value.value().StringOrDie().value());
  }

  {
    auto value = upstream[CelValue::CreateStringView(Port)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsInt64());
    EXPECT_EQ(679, value.value().Int64OrDie());
  }

  {
    auto value = connection[CelValue::CreateStringView(MTLS)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsBool());
    EXPECT_TRUE(value.value().BoolOrDie());
  }

  {
    auto value = connection[CelValue::CreateStringView(RequestedServerName)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(sni_name, value.value().StringOrDie().value());
  }

  {
    auto value = connection[CelValue::CreateStringView(TLSVersion)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(tls_version, value.value().StringOrDie().value());
  }

  {
    auto value = connection[CelValue::CreateStringView(DNSSanLocalCertificate)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(dns_sans_local[0], value.value().StringOrDie().value());
  }

  {
    auto value = connection[CelValue::CreateStringView(DNSSanPeerCertificate)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(dns_sans_peer[0], value.value().StringOrDie().value());
  }

  {
    auto value = connection[CelValue::CreateStringView(URISanLocalCertificate)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(uri_sans_local[0], value.value().StringOrDie().value());
  }

  {
    auto value = connection[CelValue::CreateStringView(URISanPeerCertificate)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(uri_sans_peer[0], value.value().StringOrDie().value());
  }

  {
    auto value = connection[CelValue::CreateStringView(SubjectLocalCertificate)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(subject_local, value.value().StringOrDie().value());
  }

  {
    auto value = connection[CelValue::CreateStringView(SubjectPeerCertificate)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(subject_peer, value.value().StringOrDie().value());
  }

  {
    auto value = upstream[CelValue::CreateStringView(TLSVersion)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(tls_version, value.value().StringOrDie().value());
  }

  {
    auto value = upstream[CelValue::CreateStringView(DNSSanLocalCertificate)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(dns_sans_local[0], value.value().StringOrDie().value());
  }

  {
    auto value = upstream[CelValue::CreateStringView(DNSSanPeerCertificate)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(dns_sans_peer[0], value.value().StringOrDie().value());
  }

  {
    auto value = upstream[CelValue::CreateStringView(URISanLocalCertificate)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(uri_sans_local[0], value.value().StringOrDie().value());
  }

  {
    auto value = upstream[CelValue::CreateStringView(URISanPeerCertificate)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(uri_sans_peer[0], value.value().StringOrDie().value());
  }

  {
    auto value = upstream[CelValue::CreateStringView(SubjectLocalCertificate)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(subject_local, value.value().StringOrDie().value());
  }

  {
    auto value = upstream[CelValue::CreateStringView(SubjectPeerCertificate)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(subject_peer, value.value().StringOrDie().value());
  }

  {
    auto value = upstream[CelValue::CreateStringView(UpstreamLocalAddress)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(upstream_local_address->asStringView(), value.value().StringOrDie().value());
  }

  {
    auto value = upstream[CelValue::CreateStringView(UpstreamTransportFailureReason)];
    EXPECT_TRUE(value.has_value());
    ASSERT_TRUE(value.value().IsString());
    EXPECT_EQ(upstream_transport_failure_reason, value.value().StringOrDie().value());
  }
}