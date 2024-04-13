str2tag (const char *str, u_char *tag)
{
  unsigned long l;
  char *endptr;
  u_int32_t t;

  if (*str == '-')
    return 0;
  
  errno = 0;
  l = strtoul (str, &endptr, 10);

  if (*endptr != '\0' || errno || l > UINT32_MAX)
    return 0;

  t = (u_int32_t) l;
  
  tag[0] = (u_char)(t >> 12);
  tag[1] = (u_char)(t >> 4);
  tag[2] = (u_char)(t << 4);

  return 1;
}