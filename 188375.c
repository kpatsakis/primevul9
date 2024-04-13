void imap_pretty_mailbox (char* path, size_t pathlen)
{
  IMAP_MBOX home, target;
  ciss_url_t url;
  char* delim;
  int tlen;
  int hlen = 0;
  char home_match = 0;

  if (imap_parse_path (path, &target) < 0)
    return;

  tlen = mutt_strlen (target.mbox);
  /* check whether we can do '=' substitution */
  if (mx_is_imap(Maildir) && !imap_parse_path (Maildir, &home))
  {
    hlen = mutt_strlen (home.mbox);
    if (tlen && mutt_account_match (&home.account, &target.account) &&
	!mutt_strncmp (home.mbox, target.mbox, hlen))
    {
      if (! hlen)
	home_match = 1;
      else if (ImapDelimChars)
	for (delim = ImapDelimChars; *delim != '\0'; delim++)
	  if (target.mbox[hlen] == *delim)
	    home_match = 1;
    }
    FREE (&home.mbox);
  }

  /* do the '=' substitution */
  if (home_match)
  {
    *path++ = '=';
    /* copy remaining path, skipping delimiter */
    if (! hlen)
      hlen = -1;
    memcpy (path, target.mbox + hlen + 1, tlen - hlen - 1);
    path[tlen - hlen - 1] = '\0';
  }
  else
  {
    mutt_account_tourl (&target.account, &url);
    url.path = target.mbox;
    url_ciss_tostring (&url, path, pathlen, 0);
  }

  FREE (&target.mbox);
}