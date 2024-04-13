static int alternative_handler (BODY *a, STATE *s)
{
  BODY *choice = NULL;
  BODY *b;
  LIST *t;
  int type = 0;
  int mustfree = 0;
  int rc = 0;

  if (a->encoding == ENCBASE64 || a->encoding == ENCQUOTEDPRINTABLE ||
      a->encoding == ENCUUENCODED)
  {
    struct stat st;
    mustfree = 1;
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

  a = b;

  /* First, search list of preferred types */
  t = AlternativeOrderList;
  while (t && !choice)
  {
    char *c;
    int btlen;  /* length of basetype */
    int wild;	/* do we have a wildcard to match all subtypes? */

    c = strchr (t->data, '/');
    if (c)
    {
      wild = (c[1] == '*' && c[2] == 0);
      btlen = c - t->data;
    }
    else
    {
      wild = 1;
      btlen = mutt_strlen (t->data);
    }

    if (a && a->parts)
      b = a->parts;
    else
      b = a;
    while (b)
    {
      const char *bt = TYPE(b);
      if (!ascii_strncasecmp (bt, t->data, btlen) && bt[btlen] == 0)
      {
	/* the basetype matches */
	if (wild || !ascii_strcasecmp (t->data + btlen + 1, b->subtype))
	{
	  choice = b;
	}
      }
      b = b->next;
    }
    t = t->next;
  }

  /* Next, look for an autoviewable type */
  if (!choice)
  {
    if (a && a->parts)
      b = a->parts;
    else
      b = a;
    while (b)
    {
      if (mutt_is_autoview (b))
	choice = b;
      b = b->next;
    }
  }

  /* Then, look for a text entry */
  if (!choice)
  {
    if (a && a->parts)
      b = a->parts;
    else
      b = a;
    while (b)
    {
      if (b->type == TYPETEXT)
      {
	if (! ascii_strcasecmp ("plain", b->subtype) && type <= TXTPLAIN)
	{
	  choice = b;
	  type = TXTPLAIN;
	}
	else if (! ascii_strcasecmp ("enriched", b->subtype) && type <= TXTENRICHED)
	{
	  choice = b;
	  type = TXTENRICHED;
	}
	else if (! ascii_strcasecmp ("html", b->subtype) && type <= TXTHTML)
	{
	  choice = b;
	  type = TXTHTML;
	}
      }
      b = b->next;
    }
  }

  /* Finally, look for other possibilities */
  if (!choice)
  {
    if (a && a->parts)
      b = a->parts;
    else
      b = a;
    while (b)
    {
      if (mutt_can_decode (b))
	choice = b;
      b = b->next;
    }
  }

  if (choice)
  {
    if (s->flags & MUTT_DISPLAY && !option (OPTWEED))
    {
      fseeko (s->fpin, choice->hdr_offset, SEEK_SET);
      mutt_copy_bytes(s->fpin, s->fpout, choice->offset-choice->hdr_offset);
    }
    mutt_body_handler (choice, s);
  }
  else if (s->flags & MUTT_DISPLAY)
  {
    /* didn't find anything that we could display! */
    state_mark_attach (s);
    state_puts(_("[-- Error:  Could not display any parts of Multipart/Alternative! --]\n"), s);
    rc = 1;
  }

  if (mustfree)
    mutt_free_body(&a);

  return rc;
}