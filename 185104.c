int HeaderMapWrapper::luaAdd(lua_State* state) {
  checkModifiable(state);

  const char* key = luaL_checkstring(state, 2);
  const char* value = luaL_checkstring(state, 3);
  headers_.addCopy(Http::LowerCaseString(key), value);
  return 0;
}