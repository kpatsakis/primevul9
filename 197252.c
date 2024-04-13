static void putinteger (lua_State *L, luaL_Buffer *b, int arg, int endian,
                        int size) {
  lua_Number n = luaL_checknumber(L, arg);
  Uinttype value;
  char buff[MAXINTSIZE];
  if (n < 0)
    value = (Uinttype)(Inttype)n;
  else
    value = (Uinttype)n;
  if (endian == LITTLE) {
    int i;
    for (i = 0; i < size; i++) {
      buff[i] = (value & 0xff);
      value >>= 8;
    }
  }
  else {
    int i;
    for (i = size - 1; i >= 0; i--) {
      buff[i] = (value & 0xff);
      value >>= 8;
    }
  }
  luaL_addlstring(b, buff, size);
}