cmd_setattr (assuan_context_t ctx, char *orig_line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc;
  char *keyword;
  int keywordlen;
  size_t nbytes;
  char *line, *linebuf;

  if ( IS_LOCKED (ctrl) )
    return gpg_error (GPG_ERR_LOCKED);

  if ((rc = open_card (ctrl, NULL)))
    return rc;

  /* We need to use a copy of LINE, because PIN_CB uses the same
     context and thus reuses the Assuan provided LINE. */
  line = linebuf = xtrystrdup (orig_line);
  if (!line)
    return out_of_core ();

  keyword = line;
  for (keywordlen=0; *line && !spacep (line); line++, keywordlen++)
    ;
  if (*line)
      *line++ = 0;
  while (spacep (line))
    line++;
  nbytes = percent_plus_unescape_inplace (line, 0);

  rc = app_setattr (ctrl->app_ctx, keyword, pin_cb, ctx,
                    (const unsigned char*)line, nbytes);
  xfree (linebuf);

  TEST_CARD_REMOVAL (ctrl, rc);
  return rc;
}