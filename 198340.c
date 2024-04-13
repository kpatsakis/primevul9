static void qp_decode_line (char *dest, char *src, size_t *l,
			    int last)
{
  char *d, *s;
  char c = 0;

  int kind = -1;
  int soft = 0;

  /* decode the line */

  for (d = dest, s = src; *s;)
  {
    switch ((kind = qp_decode_triple (s, &c)))
    {
      case  0: *d++ = c; s += 3; break;	/* qp triple */
      case -1: *d++ = *s++;      break; /* single character */
      case  1: soft = 1; s++;	 break; /* soft line break */
    }
  }

  if (!soft && last == '\n')
  {
    /* neither \r nor \n as part of line-terminating CRLF
     * may be qp-encoded, so remove \r and \n-terminate;
     * see RfC2045, sect. 6.7, (1): General 8bit representation */
    if (kind == 0 && c == '\r')
      *(d-1) = '\n';
    else
      *d++ = '\n';
  }

  *d = '\0';
  *l = d - dest;
}