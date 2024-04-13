static int qp_decode_triple (char *s, char *d)
{
  /* soft line break */
  if (*s == '=' && !(*(s+1)))
    return 1;

  /* quoted-printable triple */
  if (*s == '=' &&
      isxdigit ((unsigned char) *(s+1)) &&
      isxdigit ((unsigned char) *(s+2)))
  {
    *d = (hexval (*(s+1)) << 4) | hexval (*(s+2));
    return 0;
  }

  /* something else */
  return -1;
}