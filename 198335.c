static int run_decode_and_handler (BODY *b, STATE *s, handler_t handler, int plaintext)
{
  int origType;
  char *savePrefix = NULL;
  FILE *fp = NULL;
  BUFFER *tempfile = NULL;
  size_t tmplength = 0;
  LOFF_T tmpoffset = 0;
  int decode = 0;
  int rc = 0;

  fseeko (s->fpin, b->offset, SEEK_SET);

  /* see if we need to decode this part before processing it */
  if (b->encoding == ENCBASE64 || b->encoding == ENCQUOTEDPRINTABLE ||
      b->encoding == ENCUUENCODED || plaintext ||
      mutt_is_text_part (b))				/* text subtypes may
                                                         * require character
                                                         * set conversion even
                                                         * with 8bit encoding.
                                                         */
  {
    origType = b->type;

    if (!plaintext)
    {
      /* decode to a tempfile, saving the original destination */
      fp = s->fpout;
      tempfile = mutt_buffer_pool_get ();
      mutt_buffer_mktemp (tempfile);
      if ((s->fpout = safe_fopen (mutt_b2s (tempfile), "w")) == NULL)
      {
        mutt_error _("Unable to open temporary file!");
        dprint (1, (debugfile, "Can't open %s.\n", mutt_b2s (tempfile)));
        mutt_buffer_pool_release (&tempfile);
        return -1;
      }
      /* decoding the attachment changes the size and offset, so save a copy
       * of the "real" values now, and restore them after processing
       */
      tmplength = b->length;
      tmpoffset = b->offset;

      /* if we are decoding binary bodies, we don't want to prefix each
       * line with the prefix or else the data will get corrupted.
       */
      savePrefix = s->prefix;
      s->prefix = NULL;

      decode = 1;
    }
    else
      b->type = TYPETEXT;

    mutt_decode_attachment (b, s);

    if (decode)
    {
      b->length = ftello (s->fpout);
      b->offset = 0;
      safe_fclose (&s->fpout);

      /* restore final destination and substitute the tempfile for input */
      s->fpout = fp;
      fp = s->fpin;
      s->fpin = fopen (mutt_b2s (tempfile), "r");
      unlink (mutt_b2s (tempfile));
      mutt_buffer_pool_release (&tempfile);

      /* restore the prefix */
      s->prefix = savePrefix;
    }

    b->type = origType;
  }

  /* process the (decoded) body part */
  if (handler)
  {
    rc = handler (b, s);

    if (rc)
    {
      dprint (1, (debugfile, "Failed on attachment of type %s/%s.\n", TYPE(b), NONULL (b->subtype)));
    }

    if (decode)
    {
      b->length = tmplength;
      b->offset = tmpoffset;

      /* restore the original source stream */
      safe_fclose (&s->fpin);
      s->fpin = fp;
    }
  }
  s->flags |= MUTT_FIRSTDONE;

  return rc;
}