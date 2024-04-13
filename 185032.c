int HeaderMapWrapper::luaGet(lua_State* state) {
  const char* key = luaL_checkstring(state, 2);
  const auto value =
      Http::HeaderUtility::getAllOfHeaderAsString(headers_, Http::LowerCaseString(key));
  if (value.result().has_value()) {
    lua_pushlstring(state, value.result().value().data(), value.result().value().length());
    return 1;
  } else {
    return 0;
  }
}