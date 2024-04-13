int imap_parse_path (const char* path, IMAP_MBOX* mx)
{
  static unsigned short ImapPort = 0;
  static unsigned short ImapsPort = 0;
  struct servent* service;
  char tmp[128];
  ciss_url_t url;
  char *c;
  int n;

  if (!ImapPort)
  {
    service = getservbyname ("imap", "tcp");
    if (service)
      ImapPort = ntohs (service->s_port);
    else
      ImapPort = IMAP_PORT;
    dprint (3, (debugfile, "Using default IMAP port %d\n", ImapPort));
  }
  if (!ImapsPort)
  {
    service = getservbyname ("imaps", "tcp");
    if (service)
      ImapsPort = ntohs (service->s_port);
    else
      ImapsPort = IMAP_SSL_PORT;
    dprint (3, (debugfile, "Using default IMAPS port %d\n", ImapsPort));
  }

  /* Defaults */
  memset(&mx->account, 0, sizeof(mx->account));
  mx->account.port = ImapPort;
  mx->account.type = MUTT_ACCT_TYPE_IMAP;

  c = safe_strdup (path);
  url_parse_ciss (&url, c);
  if (url.scheme == U_IMAP || url.scheme == U_IMAPS)
  {
    if (mutt_account_fromurl (&mx->account, &url) < 0 || !*mx->account.host)
    {
      FREE (&c);
      return -1;
    }

    mx->mbox = safe_strdup (url.path);

    if (url.scheme == U_IMAPS)
      mx->account.flags |= MUTT_ACCT_SSL;

    FREE (&c);
  }
  /* old PINE-compatibility code */
  else
  {
    FREE (&c);
    if (sscanf (path, "{%127[^}]}", tmp) != 1)
      return -1;

    c = strchr (path, '}');
    if (!c)
      return -1;
    else
      /* walk past closing '}' */
      mx->mbox = safe_strdup (c+1);

    if ((c = strrchr (tmp, '@')))
    {
      *c = '\0';
      strfcpy (mx->account.user, tmp, sizeof (mx->account.user));
      strfcpy (tmp, c+1, sizeof (tmp));
      mx->account.flags |= MUTT_ACCT_USER;
    }

    if ((n = sscanf (tmp, "%127[^:/]%127s", mx->account.host, tmp)) < 1)
    {
      dprint (1, (debugfile, "imap_parse_path: NULL host in %s\n", path));
      FREE (&mx->mbox);
      return -1;
    }

    if (n > 1)
    {
      if (sscanf (tmp, ":%hu%127s", &(mx->account.port), tmp) >= 1)
	mx->account.flags |= MUTT_ACCT_PORT;
      if (sscanf (tmp, "/%s", tmp) == 1)
      {
	if (!ascii_strncmp (tmp, "ssl", 3))
	  mx->account.flags |= MUTT_ACCT_SSL;
	else
	{
	  dprint (1, (debugfile, "imap_parse_path: Unknown connection type in %s\n", path));
	  FREE (&mx->mbox);
	  return -1;
	}
      }
    }
  }

  if ((mx->account.flags & MUTT_ACCT_SSL) && !(mx->account.flags & MUTT_ACCT_PORT))
    mx->account.port = ImapsPort;

  return 0;
}