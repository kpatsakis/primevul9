cmd_setdata (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int append;
  int n, i, off;
  char *p;
  unsigned char *buf;

  append = (ctrl->in_data.value && has_option (line, "--append"));

  line = skip_options (line);

  if (locked_session && locked_session != ctrl->server_local)
    return gpg_error (GPG_ERR_LOCKED);

  /* Parse the hexstring. */
  for (p=line,n=0; hexdigitp (p); p++, n++)
    ;
  if (*p)
    return set_error (GPG_ERR_ASS_PARAMETER, "invalid hexstring");
  if (!n)
    return set_error (GPG_ERR_ASS_PARAMETER, "no data given");
  if ((n&1))
    return set_error (GPG_ERR_ASS_PARAMETER, "odd number of digits");
  n /= 2;
  if (append)
    {
      if (ctrl->in_data.valuelen + n > MAXLEN_SETDATA)
        return set_error (GPG_ERR_TOO_LARGE,
                          "limit on total size of data reached");
      buf = xtrymalloc (ctrl->in_data.valuelen + n);
    }
  else
    buf = xtrymalloc (n);
  if (!buf)
    return out_of_core ();

  if (append)
    {
      memcpy (buf, ctrl->in_data.value, ctrl->in_data.valuelen);
      off = ctrl->in_data.valuelen;
    }
  else
    off = 0;
  for (p=line, i=0; i < n; p += 2, i++)
    buf[off+i] = xtoi_2 (p);

  xfree (ctrl->in_data.value);
  ctrl->in_data.value = buf;
  ctrl->in_data.valuelen = off+n;
  return 0;
}