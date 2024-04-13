cmd_getattr (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc;
  const char *keyword;

  if ((rc = open_card (ctrl, NULL)))
    return rc;

  keyword = line;
  for (; *line && !spacep (line); line++)
    ;
  if (*line)
      *line++ = 0;

  /* (We ignore any garbage for now.) */

  /* FIXME: Applications should not return sensitive data if the card
     is locked.  */
  rc = app_getattr (ctrl->app_ctx, ctrl, keyword);

  TEST_CARD_REMOVAL (ctrl, rc);
  return rc;
}