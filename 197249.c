static int b_pack (lua_State *L) {
  luaL_Buffer b;
  const char *fmt = luaL_checkstring(L, 1);
  Header h;
  int arg = 2;
  size_t totalsize = 0;
  defaultoptions(&h);
  lua_pushnil(L);  /* mark to separate arguments from string buffer */
  luaL_buffinit(L, &b);
  while (*fmt != '\0') {
    int opt = *fmt++;
    size_t size = optsize(L, opt, &fmt);
    int toalign = gettoalign(totalsize, &h, opt, size);
    totalsize += toalign;
    while (toalign-- > 0) luaL_addchar(&b, '\0');
    switch (opt) {
      case 'b': case 'B': case 'h': case 'H':
      case 'l': case 'L': case 'T': case 'i': case 'I': {  /* integer types */
        putinteger(L, &b, arg++, h.endian, size);
        break;
      }
      case 'x': {
        luaL_addchar(&b, '\0');
        break;
      }
      case 'f': {
        float f = (float)luaL_checknumber(L, arg++);
        correctbytes((char *)&f, size, h.endian);
        luaL_addlstring(&b, (char *)&f, size);
        break;
      }
      case 'd': {
        double d = luaL_checknumber(L, arg++);
        correctbytes((char *)&d, size, h.endian);
        luaL_addlstring(&b, (char *)&d, size);
        break;
      }
      case 'c': case 's': {
        size_t l;
        const char *s = luaL_checklstring(L, arg++, &l);
        if (size == 0) size = l;
        luaL_argcheck(L, l >= (size_t)size, arg, "string too short");
        luaL_addlstring(&b, s, size);
        if (opt == 's') {
          luaL_addchar(&b, '\0');  /* add zero at the end */
          size++;
        }
        break;
      }
      default: controloptions(L, opt, &fmt, &h);
    }
    totalsize += size;
  }
  luaL_pushresult(&b);
  return 1;
}