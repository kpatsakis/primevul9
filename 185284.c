int HeaderMapWrapper::luaRemove(lua_State* state) {
  checkModifiable(state);

  const char* key = luaL_checkstring(state, 2);
  headers_.remove(Http::LowerCaseString(key));
  return 0;
}