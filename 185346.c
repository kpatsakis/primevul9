int PublicKeyWrapper::luaGet(lua_State* state) {
  if (public_key_.empty()) {
    lua_pushnil(state);
  } else {
    lua_pushstring(state, public_key_.c_str());
  }
  return 1;
}