static void enriched_flush (struct enriched_state *stte, int wrap)
{
  if (!stte->tag_level[RICH_NOFILL] && (stte->line_len + stte->word_len >
      (stte->WrapMargin - (stte->tag_level[RICH_INDENT_RIGHT] * IndentSize) -
       stte->indent_len)))
    enriched_wrap (stte);

  if (stte->buff_used)
  {
    stte->buffer[stte->buff_used] = (wchar_t) '\0';
    stte->line_used += stte->buff_used;
    if (stte->line_used > stte->line_max)
    {
      stte->line_max = stte->line_used;
      safe_realloc (&stte->line, (stte->line_max + 1) * sizeof (wchar_t));
    }
    wcscat (stte->line, stte->buffer);
    stte->line_len += stte->word_len;
    stte->word_len = 0;
    stte->buff_used = 0;
  }
  if (wrap)
    enriched_wrap(stte);
  fflush (stte->s->fpout);
}