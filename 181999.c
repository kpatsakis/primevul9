LUA_API int lua_isyieldable (lua_State *L) {
  return yieldable(L);
}