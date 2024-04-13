static int autoview_handler (BODY *a, STATE *s)
{
  rfc1524_entry *entry = rfc1524_new_entry ();
  char buffer[LONG_STRING];
  char type[STRING];
  BUFFER *command = NULL;
  BUFFER *tempfile = NULL;
  char *fname;
  FILE *fpin = NULL;
  FILE *fpout = NULL;
  FILE *fperr = NULL;
  int piped = FALSE;
  pid_t thepid;
  int rc = 0;

  command = mutt_buffer_pool_get ();
  tempfile = mutt_buffer_pool_get ();

  snprintf (type, sizeof (type), "%s/%s", TYPE (a), a->subtype);
  rfc1524_mailcap_lookup (a, type, sizeof(type), entry, MUTT_AUTOVIEW);

  fname = safe_strdup (a->filename);
  mutt_sanitize_filename (fname, MUTT_SANITIZE_ALLOW_8BIT);
  mutt_rfc1524_expand_filename (entry->nametemplate, fname, tempfile);
  FREE (&fname);

  if (entry->command)
  {
    mutt_buffer_strcpy (command, entry->command);

    /* rfc1524_expand_command returns 0 if the file is required */
    piped = mutt_rfc1524_expand_command (a, mutt_b2s (tempfile), type, command);

    if (s->flags & MUTT_DISPLAY)
    {
      state_mark_attach (s);
      state_printf (s, _("[-- Autoview using %s --]\n"), mutt_b2s (command));
      mutt_message(_("Invoking autoview command: %s"), mutt_b2s (command));
    }

    if ((fpin = safe_fopen (mutt_b2s (tempfile), "w+")) == NULL)
    {
      mutt_perror ("fopen");
      rc = -1;
      goto cleanup;
    }

    mutt_copy_bytes (s->fpin, fpin, a->length);

    if (!piped)
    {
      safe_fclose (&fpin);
      thepid = mutt_create_filter (mutt_b2s (command), NULL, &fpout, &fperr);
    }
    else
    {
      unlink (mutt_b2s (tempfile));
      fflush (fpin);
      rewind (fpin);
      thepid = mutt_create_filter_fd (mutt_b2s (command), NULL, &fpout, &fperr,
				      fileno(fpin), -1, -1);
    }

    if (thepid < 0)
    {
      mutt_perror _("Can't create filter");
      if (s->flags & MUTT_DISPLAY)
      {
	state_mark_attach (s);
	state_printf (s, _("[-- Can't run %s. --]\n"), mutt_b2s (command));
      }
      rc = 1;
      goto bail;
    }

    /* Note: only replying and forwarding use s->prefix, but just to
     * be safe, keep an explicit check for s->prefix too. */
    if ((s->flags & (MUTT_REPLYING | MUTT_FORWARDING)) || s->prefix)
    {
      /* Remove ansi and formatting from autoview output.
       * The user may want to see the formatting in the pager, but it
       * shouldn't be in their quoted reply or inline forward text too.
       */
      BUFFER *stripped = mutt_buffer_pool_get ();
      while (fgets (buffer, sizeof(buffer), fpout) != NULL)
      {
        mutt_buffer_strip_formatting (stripped, buffer, 0);
        if (s->prefix)
          state_puts (s->prefix, s);
        state_puts (mutt_b2s (stripped), s);
      }
      mutt_buffer_pool_release (&stripped);
    }
    else
    {
      mutt_copy_stream (fpout, s->fpout);
    }

    /* Check for stderr messages */
    if (fgets (buffer, sizeof(buffer), fperr))
    {
      if (s->flags & MUTT_DISPLAY)
      {
        state_mark_attach (s);
        state_printf (s, _("[-- Autoview stderr of %s --]\n"), mutt_b2s (command));
      }

      if (s->prefix)
        state_puts (s->prefix, s);
      state_puts (buffer, s);

      if (s->prefix)
      {
	while (fgets (buffer, sizeof(buffer), fperr) != NULL)
	{
	  state_puts (s->prefix, s);
	  state_puts (buffer, s);
	}
      }
      else
        mutt_copy_stream (fperr, s->fpout);
    }

  bail:
    safe_fclose (&fpout);
    safe_fclose (&fperr);

    mutt_wait_filter (thepid);
    if (piped)
      safe_fclose (&fpin);
    else
      mutt_unlink (mutt_b2s (tempfile));

    if (s->flags & MUTT_DISPLAY)
      mutt_clear_error ();
  }

cleanup:
  rfc1524_free_entry (&entry);

  mutt_buffer_pool_release (&command);
  mutt_buffer_pool_release (&tempfile);

  return rc;
}