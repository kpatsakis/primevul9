LUALIB_API int luaopen_struct (lua_State *L) {
  luaL_register(L, "struct", thislib);
  return 1;
}