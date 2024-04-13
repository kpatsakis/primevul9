int HeaderMapWrapper::luaReplace(lua_State* state) {
  checkModifiable(state);

  const char* key = luaL_checkstring(state, 2);
  const char* value = luaL_checkstring(state, 3);
  const Http::LowerCaseString lower_key(key);

  headers_.setCopy(lower_key, value);

  return 0;
}