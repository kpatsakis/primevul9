void luaD_inctop (lua_State *L) {
  luaD_checkstack(L, 1);
  L->top++;
}