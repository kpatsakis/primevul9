char *imap_fix_path (IMAP_DATA *idata, const char *mailbox, char *path,
                     size_t plen)
{
  int i = 0;
  char delim = '\0';

  if (idata)
    delim = idata->delim;

  while (mailbox && *mailbox && i < plen - 1)
  {
    if ((ImapDelimChars && strchr(ImapDelimChars, *mailbox))
        || (delim && *mailbox == delim))
    {
      /* use connection delimiter if known. Otherwise use user delimiter */
      if (!idata)
        delim = *mailbox;

      while (*mailbox
	     && ((ImapDelimChars && strchr(ImapDelimChars, *mailbox))
	         || (delim && *mailbox == delim)))
        mailbox++;
      path[i] = delim;
    }
    else
    {
      path[i] = *mailbox;
      mailbox++;
    }
    i++;
  }
  if (i && path[--i] != delim)
    i++;
  path[i] = '\0';

  return path;
}