  void expectToPrintCurrentProtocol(const absl::optional<Envoy::Http::Protocol>& protocol) {
    const std::string SCRIPT{R"EOF(
      function callMe(object)
        testPrint(string.format("'%s'", object:protocol()))
      end
    )EOF"};

    InSequence s;
    setup(SCRIPT);

    NiceMock<Envoy::StreamInfo::MockStreamInfo> stream_info;
    ON_CALL(stream_info, protocol()).WillByDefault(ReturnPointee(&protocol));
    Filters::Common::Lua::LuaDeathRef<StreamInfoWrapper> wrapper(
        StreamInfoWrapper::create(coroutine_->luaState(), stream_info), true);
    EXPECT_CALL(printer_,
                testPrint(fmt::format("'{}'", Http::Utility::getProtocolString(protocol.value()))));
    start("callMe");
    wrapper.reset();
  }