int rfc822_valid_msgid (const char *msgid)
{
  /* msg-id         = "<" addr-spec ">"
   * addr-spec      = local-part "@" domain
   * local-part     = word *("." word)
   * word           = atom / quoted-string
   * atom           = 1*<any CHAR except specials, SPACE and CTLs>
   * CHAR           = ( 0.-127. )
   * specials       = "(" / ")" / "<" / ">" / "@"
                    / "," / ";" / ":" / "\" / <">
		    / "." / "[" / "]"
   * SPACE          = ( 32. )
   * CTLS           = ( 0.-31., 127.)
   * quoted-string  = <"> *(qtext/quoted-pair) <">
   * qtext          = <any CHAR except <">, "\" and CR>
   * CR             = ( 13. )
   * quoted-pair    = "\" CHAR
   * domain         = sub-domain *("." sub-domain)
   * sub-domain     = domain-ref / domain-literal
   * domain-ref     = atom
   * domain-literal = "[" *(dtext / quoted-pair) "]"
   */

  unsigned int l, i;

  if (!msgid || !*msgid)
    return -1;

  l = mutt_strlen (msgid);
  if (l < 5) /* <atom@atom> */
    return -1;
  if (msgid[0] != '<' || msgid[l-1] != '>')
    return -1;
  if (!(strrchr (msgid, '@')))
    return -1;

  /* TODO: complete parser */
  for (i = 0; i < l; i++)
    if ((unsigned char)msgid[i] > 127)
      return -1;

  return 0;
}