void rfc822_write_address_single (char *buf, size_t buflen, ADDRESS *addr,
				  int display)
{
  size_t len;
  char *pbuf = buf;
  char *pc;

  if (!addr)
    return;

  buflen--; /* save room for the terminal nul */

#ifdef EXACT_ADDRESS
  if (addr->val)
  {
    if (!buflen)
      goto done;
    strfcpy (pbuf, addr->val, buflen);
    len = mutt_strlen (pbuf);
    pbuf += len;
    buflen -= len;
    if (addr->group)
    {
      if (!buflen)
	goto done;
      *pbuf++ = ':';
      buflen--;
      *pbuf = 0;
    }
    return;
  }
#endif

  if (addr->personal)
  {
    if (strpbrk (addr->personal, RFC822Specials))
    {
      if (!buflen)
	goto done;
      *pbuf++ = '"';
      buflen--;
      for (pc = addr->personal; *pc && buflen > 0; pc++)
      {
	if (*pc == '"' || *pc == '\\')
	{
	  *pbuf++ = '\\';
	  buflen--;
	}
	if (!buflen)
	  goto done;
	*pbuf++ = *pc;
	buflen--;
      }
      if (!buflen)
	goto done;
      *pbuf++ = '"';
      buflen--;
    }
    else
    {
      if (!buflen)
	goto done;
      strfcpy (pbuf, addr->personal, buflen);
      len = mutt_strlen (pbuf);
      pbuf += len;
      buflen -= len;
    }

    if (!buflen)
      goto done;
    *pbuf++ = ' ';
    buflen--;
  }

  if (addr->personal || (addr->mailbox && *addr->mailbox == '@'))
  {
    if (!buflen)
      goto done;
    *pbuf++ = '<';
    buflen--;
  }

  if (addr->mailbox)
  {
    if (!buflen)
      goto done;
    if (ascii_strcmp (addr->mailbox, "@") && !display)
    {
      strfcpy (pbuf, addr->mailbox, buflen);
      len = mutt_strlen (pbuf);
    }
    else if (ascii_strcmp (addr->mailbox, "@") && display)
    {
      strfcpy (pbuf, mutt_addr_for_display (addr), buflen);
      len = mutt_strlen (pbuf);
    }
    else
    {
      *pbuf = '\0';
      len = 0;
    }
    pbuf += len;
    buflen -= len;

    if (addr->personal || (addr->mailbox && *addr->mailbox == '@'))
    {
      if (!buflen)
	goto done;
      *pbuf++ = '>';
      buflen--;
    }

    if (addr->group)
    {
      if (!buflen)
	goto done;
      *pbuf++ = ':';
      buflen--;
      if (!buflen)
	goto done;
      *pbuf++ = ' ';
      buflen--;
    }
  }
  else
  {
    if (!buflen)
      goto done;
    *pbuf++ = ';';
    buflen--;
  }
done:
  /* no need to check for length here since we already save space at the
     beginning of this routine */
  *pbuf = 0;
}