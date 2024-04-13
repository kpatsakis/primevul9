void imap_cachepath (IMAP_DATA *idata, const char *mailbox, BUFFER *dest)
{
  const char *p = mailbox;

  mutt_buffer_clear (dest);
  if (!p)
    return;

  while (*p)
  {
    if (*p == idata->delim)
    {
      mutt_buffer_addch (dest, '/');
      /* simple way to avoid collisions with UIDs */
      if (*(p + 1) >= '0' && *(p + 1) <= '9')
        mutt_buffer_addch (dest, '_');
    }
    else
      mutt_buffer_addch (dest, *p);
    p++;
  }
}