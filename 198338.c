static int external_body_handler (BODY *b, STATE *s)
{
  const char *access_type;
  const char *expiration;
  time_t expire;

  access_type = mutt_get_parameter ("access-type", b->parameter);
  if (!access_type)
  {
    if (s->flags & MUTT_DISPLAY)
    {
      state_mark_attach (s);
      state_puts (_("[-- Error: message/external-body has no access-type parameter --]\n"), s);
      return 0;
    }
    else
      return -1;
  }

  expiration = mutt_get_parameter ("expiration", b->parameter);
  if (expiration)
    expire = mutt_parse_date (expiration, NULL);
  else
    expire = -1;

  if (!ascii_strcasecmp (access_type, "x-mutt-deleted"))
  {
    if (s->flags & (MUTT_DISPLAY|MUTT_PRINTING))
    {
      char *length;
      char pretty_size[10];

      state_mark_attach (s);
      state_printf (s, _("[-- This %s/%s attachment "),
                    TYPE(b->parts), b->parts->subtype);
      length = mutt_get_parameter ("length", b->parameter);
      if (length)
      {
	mutt_pretty_size (pretty_size, sizeof (pretty_size),
			  strtol (length, NULL, 10));
	state_printf (s, _("(size %s bytes) "), pretty_size);
      }
      state_puts (_("has been deleted --]\n"), s);

      if (expire != -1)
      {
	state_mark_attach (s);
	state_printf (s, _("[-- on %s --]\n"), expiration);
      }
      if (b->parts->filename)
      {
	state_mark_attach (s);
	state_printf (s, _("[-- name: %s --]\n"), b->parts->filename);
      }

      mutt_copy_hdr (s->fpin, s->fpout, ftello (s->fpin), b->parts->offset,
		     (option (OPTWEED) ? (CH_WEED | CH_REORDER) : 0) |
		     CH_DECODE , NULL);
    }
  }
  else if (expiration && expire < time(NULL))
  {
    if (s->flags & MUTT_DISPLAY)
    {
      state_mark_attach (s);
      state_printf (s, _("[-- This %s/%s attachment is not included, --]\n"),
		    TYPE(b->parts), b->parts->subtype);
      state_attach_puts (_("[-- and the indicated external source has --]\n"
			   "[-- expired. --]\n"), s);

      mutt_copy_hdr(s->fpin, s->fpout, ftello (s->fpin), b->parts->offset,
		    (option (OPTWEED) ? (CH_WEED | CH_REORDER) : 0) |
		    CH_DECODE | CH_DISPLAY, NULL);
    }
  }
  else
  {
    if (s->flags & MUTT_DISPLAY)
    {
      state_mark_attach (s);
      state_printf (s,
		    _("[-- This %s/%s attachment is not included, --]\n"),
		    TYPE (b->parts), b->parts->subtype);
      state_mark_attach (s);
      state_printf (s,
		    _("[-- and the indicated access-type %s is unsupported --]\n"),
		    access_type);
      mutt_copy_hdr (s->fpin, s->fpout, ftello (s->fpin), b->parts->offset,
		     (option (OPTWEED) ? (CH_WEED | CH_REORDER) : 0) |
		     CH_DECODE | CH_DISPLAY, NULL);
    }
  }

  return 0;
}