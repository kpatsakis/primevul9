tor_version_parse(const char *s, tor_version_t *out)
{
  char *eos=NULL;
  const char *cp=NULL;
  /* Format is:
   *   "Tor " ? NUM dot NUM dot NUM [ ( pre | rc | dot ) NUM [ - tag ] ]
   */
  tor_assert(s);
  tor_assert(out);

  memset(out, 0, sizeof(tor_version_t));

  if (!strcasecmpstart(s, "Tor "))
    s += 4;

  /* Get major. */
  out->major = (int)strtol(s,&eos,10);
  if (!eos || eos==s || *eos != '.') return -1;
  cp = eos+1;

  /* Get minor */
  out->minor = (int) strtol(cp,&eos,10);
  if (!eos || eos==cp || *eos != '.') return -1;
  cp = eos+1;

  /* Get micro */
  out->micro = (int) strtol(cp,&eos,10);
  if (!eos || eos==cp) return -1;
  if (!*eos) {
    out->status = VER_RELEASE;
    out->patchlevel = 0;
    return 0;
  }
  cp = eos;

  /* Get status */
  if (*cp == '.') {
    out->status = VER_RELEASE;
    ++cp;
  } else if (0==strncmp(cp, "pre", 3)) {
    out->status = VER_PRE;
    cp += 3;
  } else if (0==strncmp(cp, "rc", 2)) {
    out->status = VER_RC;
    cp += 2;
  } else {
    return -1;
  }

  /* Get patchlevel */
  out->patchlevel = (int) strtol(cp,&eos,10);
  if (!eos || eos==cp) return -1;
  cp = eos;

  /* Get status tag. */
  if (*cp == '-' || *cp == '.')
    ++cp;
  eos = (char*) find_whitespace(cp);
  if (eos-cp >= (int)sizeof(out->status_tag))
    strlcpy(out->status_tag, cp, sizeof(out->status_tag));
  else {
    memcpy(out->status_tag, cp, eos-cp);
    out->status_tag[eos-cp] = 0;
  }
  cp = eat_whitespace(eos);

  if (!strcmpstart(cp, "(r")) {
    cp += 2;
    out->svn_revision = (int) strtol(cp,&eos,10);
  } else if (!strcmpstart(cp, "(git-")) {
    char *close_paren = strchr(cp, ')');
    int hexlen;
    char digest[DIGEST_LEN];
    if (! close_paren)
      return -1;
    cp += 5;
    if (close_paren-cp > HEX_DIGEST_LEN)
      return -1;
    hexlen = (int)(close_paren-cp);
    memset(digest, 0, sizeof(digest));
    if ( hexlen == 0 || (hexlen % 2) == 1)
      return -1;
    if (base16_decode(digest, hexlen/2, cp, hexlen))
      return -1;
    memcpy(out->git_tag, digest, hexlen/2);
    out->git_tag_len = hexlen/2;
  }

  return 0;
}