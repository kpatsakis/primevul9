void rfc822_qualify (ADDRESS *addr, const char *host)
{
  char *p;

  for (; addr; addr = addr->next)
    if (!addr->group && addr->mailbox && strchr (addr->mailbox, '@') == NULL)
    {
      p = safe_malloc (mutt_strlen (addr->mailbox) + mutt_strlen (host) + 2);
      sprintf (p, "%s@%s", addr->mailbox, host);	/* __SPRINTF_CHECKED__ */
      FREE (&addr->mailbox);
      addr->mailbox = p;
    }
}