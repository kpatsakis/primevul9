static int b_size (lua_State *L) {
  Header h;
  const char *fmt = luaL_checkstring(L, 1);
  size_t pos = 0;
  defaultoptions(&h);
  while (*fmt) {
    int opt = *fmt++;
    size_t size = optsize(L, opt, &fmt);
    pos += gettoalign(pos, &h, opt, size);
    if (opt == 's')
      luaL_argerror(L, 1, "option 's' has no fixed size");
    else if (opt == 'c' && size == 0)
      luaL_argerror(L, 1, "option 'c0' has no fixed size");
    if (!isalnum(opt))
      controloptions(L, opt, &fmt, &h);
    pos += size;
  }
  lua_pushinteger(L, pos);
  return 1;
}