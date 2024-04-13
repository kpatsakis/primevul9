int StreamInfoWrapper::luaProtocol(lua_State* state) {
  lua_pushstring(state, Http::Utility::getProtocolString(stream_info_.protocol().value()).c_str());
  return 1;
}