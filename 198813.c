int rfc822_write_address (char *buf, size_t buflen, ADDRESS *addr, int display)
{
  char *pbuf = buf;
  size_t len = mutt_strlen (buf);

  buflen--; /* save room for the terminal nul */

  if (len > 0)
  {
    if (len > buflen)
      return pbuf - buf; /* safety check for bogus arguments */

    pbuf += len;
    buflen -= len;
    if (!buflen)
      goto done;
    *pbuf++ = ',';
    buflen--;
    if (!buflen)
      goto done;
    *pbuf++ = ' ';
    buflen--;
  }

  for (; addr && buflen > 0; addr = addr->next)
  {
    /* use buflen+1 here because we already saved space for the trailing
       nul char, and the subroutine can make use of it */
    rfc822_write_address_single (pbuf, buflen + 1, addr, display);

    /* this should be safe since we always have at least 1 char passed into
       the above call, which means `pbuf' should always be nul terminated */
    len = mutt_strlen (pbuf);
    pbuf += len;
    buflen -= len;

    /* if there is another address, and its not a group mailbox name or
       group terminator, add a comma to separate the addresses */
    if (addr->next && addr->next->mailbox && !addr->group)
    {
      if (!buflen)
	goto done;
      *pbuf++ = ',';
      buflen--;
      if (!buflen)
	goto done;
      *pbuf++ = ' ';
      buflen--;
    }
  }
done:
  *pbuf = 0;
  return pbuf - buf;
}