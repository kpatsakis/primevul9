static int multipart_handler (BODY *a, STATE *s)
{
  BODY *b, *p;
  char length[5];
  struct stat st;
  int count;
  int rc = 0;

  if (a->encoding == ENCBASE64 || a->encoding == ENCQUOTEDPRINTABLE ||
      a->encoding == ENCUUENCODED)
  {
    fstat (fileno (s->fpin), &st);
    b = mutt_new_body ();
    b->length = (LOFF_T) st.st_size;
    b->parts = mutt_parse_multipart (s->fpin,
                                     mutt_get_parameter ("boundary", a->parameter),
                                     (LOFF_T) st.st_size,
                                     ascii_strcasecmp ("digest", a->subtype) == 0);
  }
  else
    b = a;

  for (p = b->parts, count = 1; p; p = p->next, count++)
  {
    if (s->flags & MUTT_DISPLAY)
    {
      state_mark_attach (s);
      state_printf (s, _("[-- Attachment #%d"), count);
      if (p->description || p->filename || p->form_name)
      {
	state_puts (": ", s);
	state_puts (p->description ? p->description :
		    p->filename ? p->filename : p->form_name, s);
      }
      state_puts (" --]\n", s);

      mutt_pretty_size (length, sizeof (length), p->length);

      state_mark_attach (s);
      state_printf (s, _("[-- Type: %s/%s, Encoding: %s, Size: %s --]\n"),
		    TYPE (p), p->subtype, ENCODING (p->encoding), length);
      if (!option (OPTWEED))
      {
	fseeko (s->fpin, p->hdr_offset, SEEK_SET);
	mutt_copy_bytes(s->fpin, s->fpout, p->offset-p->hdr_offset);
      }
      else
	state_putc ('\n', s);
    }

    rc = mutt_body_handler (p, s);
    state_putc ('\n', s);

    if (rc)
    {
      mutt_error (_("One or more parts of this message could not be displayed"));
      dprint (1, (debugfile, "Failed on attachment #%d, type %s/%s.\n", count, TYPE(p), NONULL (p->subtype)));
    }

    if ((s->flags & MUTT_REPLYING)
        && (option (OPTINCLUDEONLYFIRST)) && (s->flags & MUTT_FIRSTDONE))
      break;
  }

  if (a->encoding == ENCBASE64 || a->encoding == ENCQUOTEDPRINTABLE ||
      a->encoding == ENCUUENCODED)
    mutt_free_body (&b);

  /* make failure of a single part non-fatal */
  if (rc < 0)
    rc = 1;
  return rc;
}