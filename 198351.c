static void enriched_putwc (wchar_t c, struct enriched_state *stte)
{
  if (stte->tag_level[RICH_PARAM])
  {
    if (stte->tag_level[RICH_COLOR])
    {
      if (stte->param_used + 1 >= stte->param_len)
	safe_realloc (&stte->param, (stte->param_len += STRING) * sizeof (wchar_t));

      stte->param[stte->param_used++] = c;
    }
    return; /* nothing to do */
  }

  /* see if more space is needed (plus extra for possible rich characters) */
  if (stte->buff_len < stte->buff_used + 3)
  {
    stte->buff_len += LONG_STRING;
    safe_realloc (&stte->buffer, (stte->buff_len + 1) * sizeof (wchar_t));
  }

  if ((!stte->tag_level[RICH_NOFILL] && iswspace (c)) || c == (wchar_t) '\0')
  {
    if (c == (wchar_t) '\t')
      stte->word_len += 8 - (stte->line_len + stte->word_len) % 8;
    else
      stte->word_len++;

    stte->buffer[stte->buff_used++] = c;
    enriched_flush (stte, 0);
  }
  else
  {
    if (stte->s->flags & MUTT_DISPLAY)
    {
      if (stte->tag_level[RICH_BOLD])
      {
	stte->buffer[stte->buff_used++] = c;
	stte->buffer[stte->buff_used++] = (wchar_t) '\010';
	stte->buffer[stte->buff_used++] = c;
      }
      else if (stte->tag_level[RICH_UNDERLINE])
      {

	stte->buffer[stte->buff_used++] = '_';
	stte->buffer[stte->buff_used++] = (wchar_t) '\010';
	stte->buffer[stte->buff_used++] = c;
      }
      else if (stte->tag_level[RICH_ITALIC])
      {
	stte->buffer[stte->buff_used++] = c;
	stte->buffer[stte->buff_used++] = (wchar_t) '\010';
	stte->buffer[stte->buff_used++] = '_';
      }
      else
      {
	stte->buffer[stte->buff_used++] = c;
      }
    }
    else
    {
      stte->buffer[stte->buff_used++] = c;
    }
    stte->word_len++;
  }
}