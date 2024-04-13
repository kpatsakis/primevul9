parse_addr_spec (const char *s,
		 char *comment, size_t *commentlen, size_t commentmax,
		 ADDRESS *addr)
{
  char token[LONG_STRING];
  size_t tokenlen = 0;

  s = parse_address (s, token, &tokenlen, sizeof (token) - 1, comment, commentlen, commentmax, addr);
  if (s && *s && *s != ',' && *s != ';')
  {
    RFC822Error = ERR_BAD_ADDR_SPEC;
    return NULL;
  }
  return s;
}