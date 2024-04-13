int StreamInfoWrapper::luaDownstreamSslConnection(lua_State* state) {
  const auto& ssl = stream_info_.downstreamSslConnection();
  if (ssl != nullptr) {
    if (downstream_ssl_connection_.get() != nullptr) {
      downstream_ssl_connection_.pushStack();
    } else {
      downstream_ssl_connection_.reset(
          Filters::Common::Lua::SslConnectionWrapper::create(state, *ssl), true);
    }
  } else {
    lua_pushnil(state);
  }
  return 1;
}