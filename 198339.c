static void enriched_wrap (struct enriched_state *stte)
{
  int x;
  int extra;

  if (stte->line_len)
  {
    if (stte->tag_level[RICH_CENTER] || stte->tag_level[RICH_FLUSHRIGHT])
    {
      /* Strip trailing white space */
      size_t y = stte->line_used - 1;

      while (y && iswspace (stte->line[y]))
      {
	stte->line[y] = (wchar_t) '\0';
	y--;
	stte->line_used--;
	stte->line_len--;
      }
      if (stte->tag_level[RICH_CENTER])
      {
	/* Strip leading whitespace */
	y = 0;

	while (stte->line[y] && iswspace (stte->line[y]))
	  y++;
	if (y)
	{
	  size_t z;

	  for (z = y ; z <= stte->line_used; z++)
	  {
	    stte->line[z - y] = stte->line[z];
	  }

	  stte->line_len -= y;
	  stte->line_used -= y;
	}
      }
    }

    extra = stte->WrapMargin - stte->line_len - stte->indent_len -
      (stte->tag_level[RICH_INDENT_RIGHT] * IndentSize);
    if (extra > 0)
    {
      if (stte->tag_level[RICH_CENTER])
      {
	x = extra / 2;
	while (x)
	{
	  state_putc (' ', stte->s);
	  x--;
	}
      }
      else if (stte->tag_level[RICH_FLUSHRIGHT])
      {
	x = extra-1;
	while (x)
	{
	  state_putc (' ', stte->s);
	  x--;
	}
      }
    }
    state_putws ((const wchar_t*) stte->line, stte->s);
  }

  state_putc ('\n', stte->s);
  stte->line[0] = (wchar_t) '\0';
  stte->line_len = 0;
  stte->line_used = 0;
  stte->indent_len = 0;
  if (stte->s->prefix)
  {
    state_puts (stte->s->prefix, stte->s);
    stte->indent_len += mutt_strlen (stte->s->prefix);
  }

  if (stte->tag_level[RICH_EXCERPT])
  {
    x = stte->tag_level[RICH_EXCERPT];
    while (x)
    {
      if (stte->s->prefix)
      {
	state_puts (stte->s->prefix, stte->s);
        stte->indent_len += mutt_strlen (stte->s->prefix);
      }
      else
      {
	state_puts ("> ", stte->s);
	stte->indent_len += mutt_strlen ("> ");
      }
      x--;
    }
  }
  else
    stte->indent_len = 0;
  if (stte->tag_level[RICH_INDENT])
  {
    x = stte->tag_level[RICH_INDENT] * IndentSize;
    stte->indent_len += x;
    while (x)
    {
      state_putc (' ', stte->s);
      x--;
    }
  }
}