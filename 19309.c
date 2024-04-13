cmd_getinfo (assuan_context_t ctx, char *line)
{
  int rc = 0;

  if (!strcmp (line, "version"))
    {
      const char *s = VERSION;
      rc = assuan_send_data (ctx, s, strlen (s));
    }
  else if (!strcmp (line, "pid"))
    {
      char numbuf[50];

      snprintf (numbuf, sizeof numbuf, "%lu", (unsigned long)getpid ());
      rc = assuan_send_data (ctx, numbuf, strlen (numbuf));
    }
  else if (!strcmp (line, "socket_name"))
    {
      const char *s = scd_get_socket_name ();

      if (s)
        rc = assuan_send_data (ctx, s, strlen (s));
      else
        rc = gpg_error (GPG_ERR_NO_DATA);
    }
  else if (!strcmp (line, "status"))
    {
      ctrl_t ctrl = assuan_get_pointer (ctx);
      int vrdr = ctrl->server_local->vreader_idx;
      char flag = 'r';

      if (!ctrl->server_local->card_removed && vrdr != -1)
	{
	  struct vreader_s *vr;

	  if (!(vrdr >= 0 && vrdr < DIM(vreader_table)))
	    BUG ();

	  vr = &vreader_table[vrdr];
	  if (vr->valid && vr->any && (vr->status & 1))
	    flag = 'u';
	}
      rc = assuan_send_data (ctx, &flag, 1);
    }
  else if (!strcmp (line, "reader_list"))
    {
#ifdef HAVE_LIBUSB
      char *s = ccid_get_reader_list ();
#else
      char *s = NULL;
#endif

      if (s)
        rc = assuan_send_data (ctx, s, strlen (s));
      else
        rc = gpg_error (GPG_ERR_NO_DATA);
      xfree (s);
    }
  else if (!strcmp (line, "deny_admin"))
    rc = opt.allow_admin? gpg_error (GPG_ERR_GENERAL) : 0;
  else if (!strcmp (line, "app_list"))
    {
      char *s = get_supported_applications ();
      if (s)
        rc = assuan_send_data (ctx, s, strlen (s));
      else
        rc = 0;
      xfree (s);
    }
  else
    rc = set_error (GPG_ERR_ASS_PARAMETER, "unknown value for WHAT");
  return rc;
}